
#include "parser_addrmode.h"

namespace bodoasm
{
    Parser_AddrMode::Parser_AddrMode(const Package& pkg, const std::string& scope, const Pattern* pat)
        : SubParser(pkg, 0)
        , curScope(scope)
        , pattern(pat)
    {
        patPos = 0;
    }

    AddrModeExprs Parser_AddrMode::parse()
    {
        while(patPos < pattern->elements.size())
        {
            //  TODO empty pattern?
            if(pattern->elements[patPos].type == Pattern::El::Type::Match)
                doMatch();
            else
                doExpr();
        }
        // at this point, all patterns have been matched.  There can't be anything else in this command
        auto t = next();
        if(!t.isEnd())
        {
            if(t.type == Token::Type::String)       error(&t.pos, "Unexpected string literal");
            else                                    error(&t.pos, "Unexpected token " + t.str);
        }

        return AddrModeExprs();
    }

    void Parser_AddrMode::doMatch()
    {
        const auto& pat = pattern->elements[patPos].match;
        std::string input;
        Token t;
        while(input.size() < pat.size())
        {
            t = next();
            if(t.type == Token::Type::String)       error(&t.pos, "Unexpeced string literal");
            if(t.isEnd())                           error(&t.pos, "Unexpected end of command reached");
            input += t.str;
            if(t.ws_after)                          break;
        }
        // TODO these positions aren't quite right but whatever
        if(input.size() > pat.size())               error(&t.pos, std::string("Unexpected character '") + input[pat.size()] + "'");
        if(input.size() < pat.size())               error(&t.pos, std::string("Missing expected character '") + pat[input.size()] + "'");
        input = toLower(input);
        if(input != pat)                            error(&t.pos, "Input '" + input + "' does not match expected pattern '" + pat + "'");

        // If we made it here, it's a proper match!
        ++patPos;
    }

    void Parser_AddrMode::doExpr()
    {
        ExprRange range;
        range.type = pattern->elements[patPos].type;
        range.start = getCurrentLexPos();

        // for now, just assume we match the expression
        ++patPos;

        // If this is the last element we're matching, we can assume the rest of the lex input
        //   is part of the expression!  Easy!
        if(patPos >= pattern->elements.size())
        {
            range.stop = getCurrentLexSize();
            exprRanges.push_back(range);
            return;
        }

        //  Otherwise, we need to find the next token that matches the start of the next element
        //  Expressions must be at least one token wide, so the next token should be non-end.
        //    Also it must belong to the expression and therefore we have to skip it
        Token t = next();
        if(t.isEnd())
            error(&t.pos, "Unexpected end of command reached");

        //  At this point, start looking for potential matches for the next element.
        //    We can assume the element is a 'Match' type, and that it is a non-empty
        //    string, but do a sanity check here just to make sure.
        if( pattern->elements[patPos].type != Pattern::El::Type::Match ||
            pattern->elements[patPos].match.empty() )
            error(&t.pos, "Internal error:  Pattern has back-to-back expressions or match string is empty");

        char toFind = pattern->elements[patPos].match.front();

        // The below loop will only exit upon error.
        //   Successful matches will be pushed into the 'forks' vector to be followed up later
        while(true)
        {
            t = next();
            if(t.isEnd())                       error(&t.pos, "Unexpected end of command reached");
            if(t.type == Token::Type::String)   error(&t.pos, "Unexpected string literal");
            if(t.str.empty())                   error(&t.pos, "Internal error:  pattern matching string is empty");

            if(toFind == t.str.front())         // possible match!
            {
                // Now, this is where it gets tricky.  This might not be a real match.  This might still be part
                //   of the expression.  So we need to effectively "fork".  We will create a "Fork" struct instance
                //   which contains all the info necessary to resume parsing from this point as if this is a match.
                //   We'll record that in a vector, and we'll proceed as if this wasn't a match.
                back();     // unget the match while building the Fork
                Fork f;
                f.exprRanges =  exprRanges;
                f.patPos =      patPos;
                f.pkg =         buildForkSubPackage();
                next();     // toss the match we ungot before

                forks.emplace_back(std::move(f));
                if(forks.size() > 300)
                    throw std::runtime_error("Fatal error:  addr mode is too complex to pattern match, try simplifying " + ErrorReporter::formatPosition(t.pos));

                // stop reporting errors in this path.  Let the fork do it (since that's more likely to match, seeing as how we followed a match)
                stopReportingErrors();
            }
        }
    }
}
