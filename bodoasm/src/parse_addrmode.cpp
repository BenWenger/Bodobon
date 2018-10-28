
#include "parser_addrmode.h"
#include "parser_expression.h"

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
        AddrModeExprs   output;

        while(patPos < pattern->elements.size())
        {
            if(pattern->elements[patPos].type == Pattern::El::Type::Match)
                doMatch();
            else
                output.emplace_back(doExpr());
        }
        // at this point, all patterns have been matched.  There can't be anything else in this command
        auto t = next();
        if(!t.isEnd())
        {
            if(t.type == Token::Type::String)       error(&t.pos, "Unexpected string literal");
            else                                    error(&t.pos, "Unexpected token " + t.str);
        }
        return output;
    }

    void Parser_AddrMode::doMatch()
    {
        //  I'm not convinced this is 100% right.  Come back to this later!
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

    AddrModeExpr Parser_AddrMode::doExpr()
    {
        auto pkg = buildForkSubPackage();
        Parser_Expression   expParser(pkg, curScope);
        
        std::size_t exprsize;
        AddrModeExpr    out;
        out.type = pattern->elements[patPos].type;
        out.expr = expParser.parse(&exprsize);

        skip(exprsize);
        ++patPos;
        return out;
    }
}
