
#include "parser.h"
#include "assembler/assembler.h"
#include "parser_expression.h"
#include "parser_addrmode.h"
#include "asmdefs/asmdefinition.h"
#include "symbolparse.h"

//  TODO --  There is a bug with #include that makes it so that if #include is the very last line (with no following line break)
//  parsing stops before the file is included

namespace bodoasm
{
    void Parser::parse(Assembler* asmblr, Lexer* lex, MacroProcessor* mac, AsmDefinition* def, SymbolTable* syms, ErrorReporter& er)
    {
        Parser p(er);
        p.assembler =   asmblr;
        p.lexer =       lex;
        p.asmDefs =     def;
        p.symbols =     syms;
        p.macroProc =   mac;

        p.macroProc->setScopePointer(&p.curScope);

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
            if(!conditionalState.empty())
                err.error(nullptr, "Unexpected end of file reached when inside an #if block");
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
        unget(t);
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
    
    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////

    void Parser::parseOne()
    {
        // Check to see if we're on the 'false' part of an #if chain
        if(!conditionalState.empty() && conditionalState.back().state != CondBlock::State::Active)
        {
            parseOneOffCondition();
            return;
        }
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
            if(!sym.ws_after && t.str == ":")                   // label definition
            {
                assembler->defineLabel(sym.pos, sym.name);
                return;
            }
            else if(t.str == "=")                               // symbol assignment
            {
                assembler->defineSymbol(sym.pos, sym.name, parse_expression());
                return;
            }
            else                                                // neither
            {                                                   // unget 't' and all tokens fetched for the symbol
                unget(t);
                sym.unget(*this);
            }
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
        unget(t);
        parse_command();
    }
    
    void Parser::parseOneOffCondition()
    {
        auto& blk = conditionalState.back();

        // when we're off condition, we only want to look for #if/#elif/#else/#endif
        auto t = next();
        if(t.str == "#")
        {
            bool docmd = false;
            auto pos = t.pos;

            t = next();
            auto str = toLower(t.str);

            if(str == "if")
            {
                CondBlock b;
                b.state = CondBlock::State::Disabled;
                conditionalState.push_back(b);
            }
            else if(str == "elif" || str == "else")
                docmd = (blk.state != CondBlock::State::Disabled);
            else
                docmd = (str == "endif");

            if(docmd)
            {
                unget(t);
                parse_directive();
                return;
            }
        }
        skipRemainderOfCommand();
    }

    SubParser::Package Parser::buildEolPackage(std::vector<Token>& owner)
    {
        do
        {
            owner.emplace_back(next());
        } while(!owner.back().isEnd());

        // keep the end in the lexer, so that if we have an error and skip to the next command, we won't
        //   skip over anything else
        unget( owner.back() );
        
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
