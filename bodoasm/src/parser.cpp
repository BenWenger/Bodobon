
#include "parser.h"
#include "assembler.h"
#include "parser_expression.h"
#include "parser_addrmode.h"
#include "asmdefinition.h"

namespace bodoasm
{
    void Parser::parse(Assembler* asmblr, Lexer* lex, AsmDefinition* def, SymbolTable* syms, ErrorReporter& er)
    {
        Parser p(er);
        p.assembler =   asmblr;
        p.lexer =       lex;
        p.asmDefs =     def;
        p.symbols =     syms;

        p.fullParse();
    }

    void Parser::fullParse()
    {
        try
        {
            bool run = true;
            while(run)
            {
                try
                {
                    run = skipEnds(true);
                    if(run)
                        parseOne();

                }catch(Error&)
                {
                    // if we hit a non-fatal error, drop everything until the end of the command
                    skipRemainderOfCommand();
                }
            }
        }
        catch(...) {}   // any other error stops all parsing
    }

    bool Parser::skipEnds(bool skipFileEnds)
    {
        Token t;
        while(true)
        {
            t = lexer->getNext();
            if(t.type == Token::Type::CmdEnd)
                continue;
            else if(t.type == Token::Type::FileEnd)
            {
                if(skipFileEnds)        continue;
                else                    break;
            }
            break;
        }
        lexer->unget(t);
        return !t.isEnd();
    }

    void Parser::skipRemainderOfCommand()
    {
        Token t;
        while(true)
        {
            try
            {
                t = lexer->getNext();
                if(t.isEnd())
                    return;
            } catch (Error&) {}
        }
    }

    Token Parser::next()
    {
        return lexer->getNext();        // TODO replace this with macro substitution
    }

    void Parser::unget(const Token& t)
    {
        lexer->unget(t);                // TODO macro substitution
    }

    void Parser::scopeChange(const std::string& newScope)
    {
        curScope = newScope;
    }
    
    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////

    void Parser::parseOne()
    {
        //  A command can be any number of label definitions, followed by one of the following:
        //   - an assembler directive       #<directive name> [...]
        //   - a symbol assignment          <symbol> = <expression>
        //   - a command                    <mnemonic> <addrmode>
        // any number of label definitions can come first, and we can treat each of them as individual commands

        auto a = next();

        if(a.str == "#")                // directive?
        {
            parse_directive();
            return;
        }
        if(a.str == ":")                // nameless label?
        {
            // TODO nameless label
            return;
        }
        
        auto b = next();
        auto c = next();

        // local symbol?
        if(a.str == "." && !a.ws_after && b.isPossibleSymbol())
        {
            if(c.str == ":")
            {
                assembler->defineLabel(a.pos, curScope + "." + b.str);
                return;
            }
            else if(c.str == "=")
            {
                assembler->defineSymbol(a.pos, curScope + "." + b.str, parse_expression());
                return;
            }
        }
        // global symbol?
        else if(a.isPossibleSymbol())
        {
            if(b.str == ":")
            {
                unget(c);
                assembler->defineLabel(a.pos, a.str);
                scopeChange(a.str);
                return;
            }
            else if(b.str == "=")
            {
                unget(c);
                assembler->defineSymbol(a.pos, a.str, parse_expression());
                return;
            }
        }

        // if we reach here, it wasn't a label/assign/directive
        //   it must be an actual command that needs pattern matching with the Lua
        unget(c);
        unget(b);
        unget(a);
        parse_command();
    }

    SubParser::Package Parser::buildEolPackage()
    {
        std::vector<Token>      tokens;
        do
        {
            tokens.emplace_back(lexer->getNext());
        } while(!tokens.back().isEnd());

        // keep the end in the lexer, so that if we have an error and skip to the next command, we won't
        //   skip over anything else
        lexer->unget( tokens.back() );

        SubParser::Package pkg;
        pkg.errReport = &err;
        pkg.tokenList = tokens.data();
        pkg.tokenListSize = tokens.size();

        return pkg;
    }

    void Parser::parse_directive()
    {
        err.fatal(nullptr, "DIRECTIVES UNSUPPORTED");
        // TODO
    }
    
    void Parser::parse_command()
    {
        // Build the mnemonic
        Position pos;
        std::string mnemonic;

        Token t = next();
        if(t.type == Token::Type::String)       err.error(&t.pos, "Unexpected string literal");
        pos = t.pos;
        mnemonic = t.str;
        while(!t.ws_after)
        {
            t = next();
            if(t.type == Token::Type::String)       err.error(&t.pos, "Unexpected string literal");
            mnemonic += t.str;
        }

        // check with the assembly def to see if this mnemonic is valid
        auto modes = asmDefs->getAddrModeForMnemonic(pos, mnemonic);
        AddrModeMatchMap        matches;        // what has matched so far
        SubParser::Package      basePackage = buildEolPackage();
        while(!modes.empty())
        {
            auto md = modes.back();
            modes.pop_back();
            // we only want to report errors to the user IF
            //  - this is the last mode we're attempting
            //  - AND we haven't found a successful match yet
            if(matches.empty() && modes.empty())
                basePackage.errReport = &err;
            else
                basePackage.errReport = nullptr;
            
            try
            {
                Parser_AddrMode p(basePackage, curScope, asmDefs->getPatternForAddrMode(md) );
                auto match = p.parse();
                matches.insert( std::make_pair( md, std::move(match) ) );
            }catch(Error&){}
        }
        /*
        Parser_AddrMode p(buildEolPackage(), curScope);
        p.parse(patterns);*/
    }
    
    Expression::Ptr Parser::parse_expression()
    {
        Parser_Expression p(buildEolPackage(), curScope);
        return p.parse();
    }
}
