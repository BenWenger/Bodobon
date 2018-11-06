
#include "parser.h"
#include "types/blocktypes.h"
#include "symbolparse.h"
#include "symbols/symboltable.h"

namespace bodoasm
{
    void Parser::getStartMacroParamList(Macro& macro)       // TODO - make sure multiple parameters don't share a name
    {
        // sort of special case for no parameters
        skipEnds();
        Token t = next();
        if(t.str == ")")            return;
        unget(t);

        while(true)
        {
            // skip over EOLs (this can span multiple lines)
            //   next token should be a symbol
            skipEnds();
            t = next();
            if(t.isEnd())                       err.error(&t.pos, "Unexpected EOF reached before closing parens");
            if(!t.isPossibleSymbol())           err.error(&t.pos, "Invalid name for macro parameter '" + t.str + "'");
            macro.paramNames.push_back(t.str);

            // skip ends again, next token should be either a comma (keep looping), or a close paren
            skipEnds();
            t = next();
            if(t.isEnd())                       err.error(&t.pos, "Unexpected EOF reached before closing parens");
            if(t.str == ")")                    break;
            if(t.str != ",")                    err.error(&t.pos, "Unexpected '" + t.str + "' in macro parameter list");
        }
    }

    void Parser::startMacroDef(const Position& pos)
    {
        // First is the macro name
        auto name = SymbolParse::parseDec(*this, curScope.topLabel);
        if(!name)
            err.error(&name.pos, "#macro directive first parameter must be the symbol name");

        Macro macro;
        macro.definePos = pos;

        // next token needs to be either an open paren (to indicate start of param list), or
        //   end of command (no params)
        auto t = next();
        if(t.str == "(")
        {
            getStartMacroParamList(macro);
        }
        else if(t.isEnd())
        {
            if(t.type != Token::Type::CmdEnd)   err.error(&t.pos, "Unexpected end of file reached in macro definition");
        }
        else                                    err.error(&t.pos, "Unexpected token '" + t.str + "'");
        
        // now start pulling and recording tokens until we reach EOF or #endmacro
        while(true)
        {
            t = next();
            if(t.isEoF())       err.error(&t.pos, "Unexpected EOF reached in macro definition");

            if(t.str == "#")
            {
                auto tmp = next();
                auto str = toLower(tmp.str);
                if(str == "macro")      err.error(&t.pos, "Cannot nest macro definitions");
                if(str == "endmacro")   break;
                unget(tmp);
            }
            macro.tokens.push_back( t );
        }

        // drop the first and last CmdEnd tokens for reasons
        if(!macro.tokens.empty() && (macro.tokens.back().type == Token::Type::CmdEnd))
            macro.tokens.pop_back();
        if(!macro.tokens.empty() && (macro.tokens.front().type == Token::Type::CmdEnd))
            macro.tokens.erase( macro.tokens.begin() );

        symbols->addMacro(name.name, std::move(macro));
    }

    void Parser::invokeMacro(const Token& backtick)
    {
        /* I've tried this like 40 times.  The [new] plan, which should work:

        - single pass
        - recursively expand macros as you come across them, with a maximum depth to catch cycles.
        - each expansion produces a vector<Token>
        - while expanding replace parameter instances with their provided arguments
        - once expanding is complete, return the vector, which will be output with no further processing
        
        Some consequences of this (using C-style macros):
            --------------------------
            #define foo()       x
            #define bar(x)      foo()
            bar(1)              // <- expands to x, not to 1
            -------------------------
            #define foo(a, b)   a
            foo(b, a)           // <- expands to b (not a cycle -- once the parameter substitution is done, you move on)
            -------------------------
            #define die(a,b)    x
            #define foo()       die( 1,
            #define bar()       2 )
            foo() bar()         // <- foo expands to:   die( 1,
                                //      at which point, 'die( 1' attempts to be expanded recursively, but can't be, as
                                //      it is incomplete, causing an error.
                                //  One might expect this to expand to die(1,2) -> x ... but it will not


        Since we're effectively just building a linear list of tokens, I'll probably have the same 'output' vector
        that just gets passed forward to all recursive expansions for them to tack on their tokens.

        Then, to actually feed back to the main parser, 'unget' all tokens in reverse order.  Which might be inefficient,
        but I'll be damned if it's not simple.
        */
        MacroInvocation         invoc = buildMacroInvocation(*this, backtick);

        std::vector<Token>      output;
        expandMacro(output, invoc, std::make_shared<Position>(backtick.pos), 0);

        for(auto i = output.rbegin(); i != output.rend(); ++i)
            unget(*i);
    }
    
    void Parser::expandMacro(std::vector<Token>& output, const MacroInvocation& invoc, const std::shared_ptr<Position>& pos, int recurseCount) const
    {
        //  TODO  -- figure out the position stack.  I think I'm doing it wrong.
        if(recurseCount >= maxMacroRecursionCount)      err.error(pos.get(), "Maximum macro recursion depth reached.  Do you have a circular dependency?");

        SubParser::Package pkg;
        pkg.errReport = &err;
        pkg.tokenList = invoc.macro->tokens.data();
        pkg.tokenListSize = invoc.macro->tokens.size();
        SubParser src(pkg, 1);
        while(true)
        {
            auto t = src.next();
            if(t.isEoF())       break;

            if(t.isPossibleSymbol())        // parameter substitution?
            {
                auto arg = invoc.args.find(t.str);
                if(arg != invoc.args.end())
                {
                    output.insert( output.end(), arg->second.begin(), arg->second.end() );      // TODO - capture position stack
                    continue;
                }
            }
            else if(t.str == "`")           // recursive macro substitution?
            {
                auto inv = buildMacroInvocation(src, t);        // TODO finish
                //expandMacro(output, inv, 
            }
            // TODO finish this
        }
    }
    
    auto Parser::buildMacroInvocation(TokenSource& src, const Token& backtick) const -> MacroInvocation
    {
        auto macName = SymbolParse::parseRef(src, curScope.topLabel);
        if(!macName)        err.error(&macName.pos, "Expected macro name to follow backtick operator");
        auto macro = symbols->getMacro(macName.name);
        if(!macro)          err.error(&macName.pos, "'" + macName.name + "' is not a defined macro name");

        std::vector<std::vector<Token>>     fullArgList;
        // Does the invocation have an argument list?
        auto t = src.next();
        if(t.str == "(")
        {
            std::vector<Token>      args;
            t = src.next();
            if(t.str != ")")
            {
                // non-empty argument list
                src.unget(t);
                bool keepLooping = true;
                while(keepLooping)
                {
                    t = src.next();
                    if(t.isEoF())       err.error(&t.pos, "Unexpected EOF reached inside macro argument list");
                    if(t.str == "/" && !t.ws_after)
                    {
                        // special escapes?!?!
                        auto esc = src.next();
                        if(esc.str == "," || esc.str == ")")
                        {
                            esc.pos = t.pos;
                            args.emplace_back(std::move(esc));
                        }
                        else
                            src.unget(esc);
                    }
                    else if(t.str == "," || t.str == ")")
                    {
                        if(args.empty())    err.error(&t.pos, "Unexpected '" + t.str + "' in macro argument list.  Macro arguments cannot be empty");
                        fullArgList.emplace_back(std::move(args));
                        args.clear();
                        if(t.str == ")")
                            keepLooping = false;
                    }
                    else
                        args.emplace_back( std::move(t) );
                }
            }
        }
        else // if(t.str != "(") 
            src.unget(t);

        // At this point, fullArgList has all the arg tokens
        if(fullArgList.size() != macro->paramNames.size())
            err.error(&backtick.pos, "'" + macName.name + "' macro invoked with " + std::to_string(fullArgList.size()) + " parameters, but was expecting "
                                         + std::to_string(macro->paramNames.size()) );
        
        // build and output MacroInvocation instance
        MacroInvocation out;
        out.invokePos = std::make_shared<Position>(backtick.pos);
        out.macro = macro;
        for(std::size_t i = 0; i < fullArgList.size(); ++i)
        {
            const std::string& name = macro->paramNames[i];
            out.args.insert( { name, std::move(fullArgList[i]) } );
        }

        return out;
    }

    /*
    Token Parser::invokeMacro(const Token& backtick)
    {
        if(macroStack.size() > 30)          // number is arbitrary
            err.error(&backtick.pos, "Maximum macro expansion depth exceeded.  Do you have a circular dependency?");

        auto macName = SymbolParse::parseRef(*this, curScope.topLabel);
        if(!macName)        err.error(&macName.pos, "Expected macro name to follow backtick operator");
        auto macro = symbols->getMacro(macName.name);
        if(!macro)          err.error(&macName.pos, "'" + macName.name + "' is not a defined macro name");

        auto mpb =              std::make_unique<MacroPlayback>();
        mpb->mac =              macro;
        mpb->tokenPos =         0;
        mpb->invokePos =        std::make_shared<Position>(backtick.pos);
        mpb->outputtingArg =    false;
        if(!macroStack.empty())
            mpb->invokePos->callStack = macroStack.back()->invokePos;

        std::vector<std::vector<Token>>     fullArgList;
        // Does the invocation have an argument list?
        auto t = next();
        if(t.str == "(")
        {
            std::vector<Token>      args;
            t = next();
            if(t.str != ")")
            {
                // non-empty argument list
                unget(t);
                bool keepLooping = true;
                while(keepLooping)
                {
                    t = next();
                    if(t.isEoF())       err.error(&t.pos, "Unexpected EOF reached inside macro argument list");
                    if(t.str == "/" && !t.ws_after)
                    {
                        // special escapes?!?!
                        auto esc = next();
                        if(esc.str == "," || esc.str == ")")
                        {
                            esc.pos = t.pos;
                            args.emplace_back(std::move(esc));
                        }
                        else
                            unget(esc);
                    }
                    else if(t.str == "," || t.str == ")")
                    {
                        if(args.empty())    err.error(&t.pos, "Unexpected '" + t.str + "' in macro argument list.  Macro arguments cannot be empty");
                        fullArgList.emplace_back(std::move(args));
                        args.clear();
                        if(t.str == ")")
                            keepLooping = false;
                    }
                    else
                        args.emplace_back( std::move(t) );
                }
            }
        }
        else // if(t.str != "(") 
            unget(t);

        // At this point, fullArgList has all the arg tokens
        if(fullArgList.size() != macro->paramNames.size())
            err.error(&backtick.pos, "'" + macName.name + "' macro invoked with " + std::to_string(fullArgList.size()) + " parameters, but was expecting "
                                         + std::to_string(macro->paramNames.size()) );
        
        // move the arg list to the mpb, arranged by param name
        for(std::size_t i = 0; i < fullArgList.size(); ++i)
        {
            const std::string& name = macro->paramNames[i];
            mpb->arguments.insert( { name, std::move(fullArgList[i]) } );
        }
        macroStack.emplace_back( std::move(mpb) );

        // Then return the first thing from the macro.
        return fetchMacroToken();
    }
    
    Token Parser::fetchMacroToken()
    {
        while(true)
        {
            if(macroStack.empty())      return next();
            auto& mpb = *macroStack.back();

            // outputting one of the arguments?
            if(mpb.outputtingArg)
            {
                if(mpb.argIter != mpb.argIterEnd)
                    return nestTokenInMacro(*(mpb.argIter++), mpb);
                else
                    mpb.outputtingArg = false;
            }
            // outputting macro body?
            if(mpb.tokenPos < mpb.mac->tokens.size())
            {
                Token t = mpb.mac->tokens[mpb.tokenPos++];

                // is it a parameter?
                auto iter = mpb.arguments.find(t.str);
                if(iter == mpb.arguments.end())             // no?
                    return nestTokenInMacro(t, mpb);

                // otherwise, start outputting the arg
                mpb.outputtingArg = true;
                mpb.argIter = iter->second.begin();
                mpb.argIterEnd = iter->second.end();
                continue;                                   // done in next iteration of loop
            }
            
            // code here reached if we are at the end of our current macro playback.
            //   so pop it!
            macroStack.pop_back();
        }
        
        throw std::runtime_error("Internal error:  Unreachable code reached in Parser::fetchMacroToken");
        return Token();
    }
    
    inline Token Parser::nestTokenInMacro(Token t, const MacroPlayback& mpb)
    {
        t.pos.callStack = mpb.invokePos;
        return t;
    }

    */
}
