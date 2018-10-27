
#include "parser.h"
#include "assembler.h"
#include "parser_expression.h"

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

    void Parser::parse_directive()
    {
        err.fatal(nullptr, "DIRECTIVES UNSUPPORTED");
        // TODO
    }
    
    void Parser::parse_command()
    {
        err.fatal(nullptr, "COMMANDS UNSUPPORTED");
        // TODO
    }
    
    Expression::Ptr Parser::parse_expression()
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

        Parser_Expression p(pkg, curScope);
        return p.parse();
    }
}
