
#include "parser.h"

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
            else if(t.type == Token::Type::InputEnd)
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
                // TODO local label def
                return;
            }
            else if(c.str == "=")
            {
                // TODO make sure this isn't ==
                // TODO local symbol assign
                return;
            }
        }
        // global symbol?
        else if(a.isPossibleSymbol())
        {
            if(b.str == ":")
            {
                // TODO global label def
                return;
            }
            else if(b.str == "=")
            {
                // TODO make sure this isn't ==
                // TODO global symbol assign
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

    void Parser::parse_directive()
    {
        // TODO
    }
    
    void Parser::parse_command()
    {
        // TODO
    }
}
