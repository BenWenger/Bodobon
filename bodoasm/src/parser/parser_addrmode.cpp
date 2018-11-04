
#include "parser_addrmode.h"
#include "parser_expression.h"
#include <algorithm>

namespace bodoasm
{
    namespace
    {
        class AmbiguiousError {};

        typedef AddrModeExprs       Success;
    }

    Parser_AddrMode::Parser_AddrMode(const Package& pkg, const Scope& scope, const Pattern& pat)
        : SubParser(pkg, 100)
        , curScope(scope)
        , pattern(pat)
    {
        root = this;
        patPos = 0;
    }

    void Parser_AddrMode::done()
    {
        if(successful)          throw successfulOutput;
        else                    throw Error();
    }
    
    void Parser_AddrMode::addSuccess()
    {
        if(successful)
            throw AmbiguiousError();

        successful = true;
        successfulOutput = std::move(output);
    }

    AddrModeExprs Parser_AddrMode::parse()
    {
        // peek at the first token just to get the position for error reporting
        auto errPosition = next().pos;
        back();

        try
        {
            doParse();
        }
        catch(Success& e)
        {
            return e;
        }
        catch(Error&)
        {
            error(&errPosition, "Unable to pattern match instruction");
        }
        catch(AmbiguiousError&)
        {
            error(&errPosition, "Instruction is ambiguous.  Pattern was matched multiple ways");
        }
        throw std::runtime_error("Internal error:  Impossible path reached");
    }
    
    void Parser_AddrMode::doParse()
    {
        while(patPos < pattern.size())
        {
            if(pattern[patPos].type == PatEl::Type::Match)
                doMatch();
            else
                doExpr();
            ++patPos;
        }
        // end of command must immediately follow
        auto t = next();
        back();
        if(!t.isEnd())
            done();
        
        // Otherwise, we're golden
        addSuccess();
        done();
    }
    
    void Parser_AddrMode::doMatch()
    {
        //  I'm not convinced this is 100% right.  Come back to this later!
        const auto& pat = pattern[patPos].match;
        std::string input;
        Token t;
        while(input.size() < pat.size())
        {
            t = next();
            if(t.type == Token::Type::String)       done();
            if(t.isEnd())                           done();
            input += t.str;
            if(t.ws_after)                          break;
        }

        input = toLower(input);
        if(input != pat)                            done();
    }

    void Parser_AddrMode::doExpr()
    {
        // If this is the last element we're matching, we can assume the rest of the lex input
        //   is part of the expression!  Easy!
        if(patPos >= pattern.size()-1)
        {
            auto adv = doExpressionParse(getCurrentLexPos(), getCurrentLexSize(), pattern[patPos].type);
            advance(adv);
            return;
        }

        auto exprStartToken = getCurrentLexPos();

        //  Otherwise, we need to find the next token that matches the start of the next element
        //  Expressions must be at least one token wide, so the next token should be non-end.
        //    Also it must belong to the expression and therefore we have to skip it
        Token t = next();
        if(t.isEnd())       done();

        //  At this point, start looking for potential matches for the next element.
        //    We can assume the element is a 'Match' type, and that it is a non-empty
        //    string, but do a sanity check here just to make sure.
        if( pattern[patPos+1].type != PatEl::Type::Match || pattern[patPos+1].match.empty() )
            fatal(&t.pos, "Internal error:  Pattern has back-to-back expressions or match string is empty");

        char toFind = toLower(pattern[patPos+1].match.front());

        while(true)
        {
            t = next();
            if(t.isEnd())                       done();
            if(t.type == Token::Type::String)   continue;           // string literals must be part of the expression
            if(t.str.empty())                   done();
            if(toFind == toLower(t.str.front()))         // possible match!
            {
                back();     // unget the pattern match
                auto exprStopToken = getCurrentLexPos();
                tryForkToPatternMatch(exprStartToken, exprStopToken, pattern[patPos].type);
                next();     // toss pattern match and continue
            }
        }
    }

    void Parser_AddrMode::tryForkToPatternMatch(std::size_t exprStart, std::size_t exprStop, PatEl::Type type)
    {
        // peek at the first token just to get the position for error reporting
        auto errPosition = next().pos;
        back();

        RecursionMarker boogiewoogiefengshui(root, &errPosition);

        Parser_AddrMode forked(*this);
        try
        {
            doExpressionParse(exprStart, exprStop, type);
            forked.patPos++;
            forked.doParse();
        }
        catch(Success& s)
        {
            output.insert( output.end(), s.begin(), s.end() );      // append 's' to our output
            addSuccess();
        }
        catch(Error&)
        {}
    }

    std::size_t Parser_AddrMode::doExpressionParse(std::size_t start, std::size_t stop, PatEl::Type type)
    {
        auto pkg = buildSubPackage(start, stop);
        pkg.errReport = nullptr;

        AddrModeExpr res;
        res.type = type;

        std::size_t ret = 0;
        Parser_Expression p(pkg, curScope);
        res.expr = p.parse(&ret);

        output.emplace_back( std::move(res) );
        return ret;
    }
}
