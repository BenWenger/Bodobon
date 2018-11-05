
#include "parser.h"
#include "assembler/assembler.h"
#include "parser_expression.h"
#include "parser_addrmode.h"
#include "asmdefs/asmdefinition.h"
#include "symbolparse.h"

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
            t = next();
            if(t.type == Token::Type::CmdEnd)
                continue;
            else if(t.type == Token::Type::FileEnd)
            {
                if(skipFileEnds)        continue;
                else                    break;
            }
            break;
        }
        back();
        return !t.isEnd();
    }

    void Parser::skipRemainderOfCommand()
    {
        Token t;
        while(true)
        {
            try
            {
                t = next();
                if(t.isEnd())
                    return;
            } catch (Error&) {}
        }
    }

    Token Parser::fetchToken()
    {
        Token out;
        if(macroStack.empty())
            out = lexer->next();
        else
            out = fetchMacroToken();

        if(out.str == "`")
            out = invokeMacro(out);
        return out;
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

        // check for a symbol name first
        auto sym = SymbolParse::parseDec(*this, curScope.topLabel);
        if(sym)
        {
            auto t = next();
            if(t.str == ":")            // label definition
            {
                assembler->defineLabel(sym.pos, sym.name);
                return;
            }
            else if(t.str == "=")       // symbol assignment
            {
                assembler->defineSymbol(sym.pos, sym.name, parse_expression());
                return;
            }
            else                                                // neither
                TokenSource::back( sym.tokensConsumed + 1 );    // unget 't' and all the tokens fetched for the symbol
        }

        auto t = next();

        if(t.str == "#")                // directive?
        {
            parse_directive();
            return;
        }
        if(t.str == ":")                // nameless label?
        {
            assembler->defineLabel( t.pos, curScope.getNamelessName() );
            curScope.nextUnnamed++;
            return;
        }

        // if we reach here, it wasn't a label/assign/directive
        //   it must be an actual command that needs pattern matching with the Lua
        back();     // drop 't'
        parse_command();
    }

    SubParser::Package Parser::buildEolPackage(std::vector<Token>& owner)
    {
        do
        {
            owner.emplace_back(next());
        } while(!owner.back().isEnd());

        // keep the end in the lexer, so that if we have an error and skip to the next command, we won't
        //   skip over anything else
        back();
        
        SubParser::Package pkg;
        pkg.errReport = &err;
        pkg.tokenList = owner.data();
        pkg.tokenListSize = owner.size();

        return pkg;
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
        std::vector<Token>      tokenBuffer;
        SubParser::Package      basePackage = buildEolPackage(tokenBuffer);
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
                Parser_AddrMode p(basePackage, curScope, *asmDefs->getPatternForAddrMode(md) );
                auto match = p.parse();
                matches.insert( std::make_pair( md, std::move(match) ) );
            }catch(Error&) {}
        }

        if(!matches.empty())
            assembler->doInstruction(pos, mnemonic, std::move(matches));
    }
    
    Expression::Ptr Parser::parse_expression()
    {
        std::vector<Token>  rawTokens;
        Parser_Expression p(buildEolPackage(rawTokens), curScope);
        return p.parse();
    }
}