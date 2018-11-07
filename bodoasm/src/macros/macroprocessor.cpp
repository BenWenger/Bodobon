
#include "lexer/lexer.h"
#include "macroprocessor.h"
#include "parser/symbolparse.h"
#include <set>

namespace bodoasm
{
    MacroProcessor::Recurse::Recurse(MacroProcessor* mc, const Position& pos) : obj(mc)
    {
        ++obj->recursionDepth;
        if(obj->recursionDepth > obj->maxRecursionDepth)
            obj->err.error(&pos, "Maximum macro expansion depth reached. Do you have a circular dependency?");
    }
    MacroProcessor::Recurse::~Recurse()
    {
        --obj->recursionDepth;
    }

    ////////////////////////////////////////////////////////

    MacroProcessor::MacroProcessor(ErrorReporter& e, Lexer* lex)
        : err(e), lexer(lex), scope(nullptr)
    {
        recursionDepth = 0;
    }
    
    void MacroProcessor::addMacro(const std::string& name, Macro&& mac)
    {
        auto pos = mac.definePos;
        auto result = macros.insert( {name, std::make_unique<Macro>( std::move(mac) ) } );
        if(!result.second)
            err.error(&pos, "Macro name '" + name + "' was already defined here: " + ErrorReporter::formatPosition(pos));
    }

    const Macro* MacroProcessor::getMacro(const std::string& name)
    {
        auto pos = macros.find(name);
        if(pos == macros.end())     return nullptr;
        return pos->second.get();
    }

    void MacroProcessor::unget(const Token& t)
    {
        lexer->unget(t);
    }

    Token MacroProcessor::next()
    {
        Token t = lexer->next();
        while(t.str == "`")
        {
            if(t.macroExpandDepth >= maxRecursionDepth)
                err.error(&t.pos, "Maximum macro recursion depth reached. Do you have a circular dependency?");
            expandMacro(t);
            t = lexer->next();
        }
        return t;
    }

    Token MacroProcessor::nextSkipEnd()
    {
        Token t;
        do
        { 
            t = next();
        }while(t.type == Token::Type::CmdEnd);
        return t;
    }

    void MacroProcessor::defineMacro(const Position& definePos)
    {
        Token t;
        Macro macro;
        macro.definePos =   definePos;

        // get name
        auto name = SymbolParse::parseDec(*this, scope->topLabel);
        if(!name)   err.error(&name.pos, "Invalid name for macro");

        // get param names
        t = next();
        if(t.type != Token::Type::CmdEnd)   // CmdEnd is OK, it'll just be an empty param list
        {
            if(t.str != "(")    err.error(&t.pos, "Expected open parens");

            t = nextSkipEnd();
            if(t.str != ")")    // if close parens, there are no params
            {
                unget(t);
                std::set<std::string>       usedParamNames;
                while(true)
                {
                    t = nextSkipEnd();
                    if(!t.isPossibleSymbol())       err.error(&t.pos, "Macro parameter name invalid or missing");
                    auto i = usedParamNames.insert(t.str);
                    if(!i.second)                   err.error(&t.pos, "Cannot have multiple macro parameters with the same name");
                    macro.paramNames.push_back(t.str);

                    t = nextSkipEnd();
                    if(t.str == ")")        break;
                    else if(t.str != ",")   err.error(&t.pos, "Unexpected token '" + t.str + "'");
                }
            }
        }

        // get body
        while(true)
        {
            t = lexer->next();
            if(t.isEoF())           err.error(&t.pos, "Unexpected EOF in macro definition");

            if(t.str == "#")
            {
                auto tmp = lexer->next();
                if(toLower(tmp.str) == "endmacro")      break;
                if(toLower(tmp.str) == "macro")         err.error(&t.pos, "Cannot nest macro definitions");
                lexer->unget(tmp);
            }
            macro.tokens.emplace_back( std::move(t) );
        }

        addMacro(name.name, std::move(macro));
    }

    void MacroProcessor::expandMacro(const Token& backtick)
    {
        int maclvl = backtick.macroExpandDepth + 1;
        if(maclvl >= maxRecursionDepth)
            err.error(&backtick.pos, "Maximum macro recursion depth reached.  Do you have a circular dependency?");
        Recurse     checkYoSelf(this, backtick.pos);

        // get macro name
        auto name = SymbolParse::parseRef(*this, scope->topLabel);
        auto macro = getMacro(name.name);
        if(!macro)
            err.error(&name.pos, "'" + name.name + "' is not a valid macro name");

        // get arg list
        auto argList = getArgList(name.name, *macro);

        // start copying Tokens!
        std::vector<Token>      tokens;
        for(auto& i : macro->tokens)
        {
            if(i.isPossibleSymbol())
            {
                auto iter = argList.find(i.str);
                if(iter != argList.end())
                {
                    for(auto& j : iter->second)
                        tokens.push_back(j);
                    tokens.back().ws_after = i.ws_after;
                    continue;
                }
            }
            tokens.push_back(i);
        }

        // output!
        Position::Ptr subPos = std::make_shared<Position>(backtick.pos);
        for(auto i = tokens.rbegin(); i != tokens.rend(); ++i)
        {
            i->pos.callStack = subPos;
            i->macroExpandDepth = maclvl;
            lexer->unget(*i);
        }
    }

    auto MacroProcessor::getArgList(const std::string& macname, const Macro& macro) -> argList_t
    {
        auto paren = next();
        if(paren.str != "(")        // no args
        {
            unget(paren);
            if(!macro.paramNames.empty())   err.error(&paren.pos, "Macro '" + macname + "' missing argument list");
            return argList_t();
        }

        // another special case for no args
        paren = nextSkipEnd();
        if(paren.str == ")")
        {
            if(!macro.paramNames.empty())   err.error(&paren.pos, "Macro '" + macname + "' missing argument list");
            return argList_t();
        }
        unget(paren);

        ordArgList_t        ordArgs;
        std::vector<Token>  arg;
        Token t;
        bool keepLooping = true;
        while(keepLooping)
        {
            while(true)
            {
                t = next();
                if(t.isEoF())           err.error(&t.pos, "Unexpected EOF in macro argument list");
                if(t.str == "\\" && !t.ws_after)        // some basic escape characters
                {
                    auto tmp = next();
                    if(tmp.str == "," || tmp.str == ")")
                        t = tmp;
                    else
                        unget(tmp);
                }
                else if(t.str == "," || t.str == ")")
                {
                    if(arg.empty())     err.error(&t.pos, "Unexpected token '" + t.str + "'");
                    keepLooping = (t.str == ",");
                    break;
                }
                arg.push_back(t);
            }
            ordArgs.emplace_back(std::move(arg));
            arg.clear();
        }

        // must have proper number of args
        if(ordArgs.size() != macro.paramNames.size())
        {
            err.error(&t.pos, "Macro '" + macname + "' expecting " + std::to_string(macro.paramNames.size()) +
                                " arguments, but " + std::to_string(ordArgs.size()) + " provided.");
        }

        argList_t out;
        for(std::size_t i = 0; i < ordArgs.size(); ++i)
            out[macro.paramNames[i]] = std::move(ordArgs[i]);

        return out;
    }


}
