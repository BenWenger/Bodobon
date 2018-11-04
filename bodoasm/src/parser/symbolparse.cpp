
#include "symbolparse.h"
#include "lexer/tokensource.h"

namespace bodoasm
{
    SymbolParse SymbolParse::parseDec(TokenSource& src, const std::string& topPrefix)
    {
        // Declarations can be top level:           symbolName
        // Or local:                                .symbolName
        //   but not a fully qualified symbol

        SymbolParse out;
        auto t = src.next();
        // local?
        if(t.str == "." && !t.ws_after)
        {
            auto name = src.next();
            if(name.isPossibleSymbol())
            {
                out.name = topPrefix + "." + name.str;
                out.type = Type::Local;
                out.ws_after = name.ws_after;
                out.pos = t.pos;
                out.tokensConsumed = 2;
            }
            else
                src.back(2);    // unget 't' and 'name'
        }
        else if(t.isPossibleSymbol())
        {
            out.name = t.str;
            out.type = Type::Top;
            out.ws_after = t.ws_after;
            out.pos = t.pos;
            out.tokensConsumed = 1;
        }
        else
            src.back();         // unget 't'

        return out;
    }

    SymbolParse SymbolParse::parseRef(TokenSource& src, const std::string& topPrefix)
    {
        // References can be top level:             sym
        // Or local:                                .sym
        // Or fully qualified:                      sym.sym
        SymbolParse out;

        auto t = src.next();
        out.pos =               t.pos;
        out.tokensConsumed =    1;

        if(t.str == "." && !t.ws_after)
        {
            ++out.tokensConsumed;
            auto name = src.next();
            if(name.isPossibleSymbol())
            {
                out.name = topPrefix + "." + name.str;
                out.type = Type::Local;
                out.ws_after = name.ws_after;
            }
        }
        else if(t.isPossibleSymbol())
        {
            if(t.ws_after)      // if there's whitespace after, there's not a dot
            {
                out.name = t.str;
                out.type = Type::Top;
                out.ws_after = true;
            }
            else
            {
                ++out.tokensConsumed;
                auto dot = src.next();
                if(dot.str == "." && !dot.ws_after)
                {
                    ++out.tokensConsumed;
                    auto lclname = src.next();
                    if(lclname.isPossibleSymbol())
                    {
                        out.type = Type::Full;
                        out.ws_after = lclname.ws_after;
                        out.name = t.str + "." + lclname.str;
                    }
                }
            }
        }

        // If we didn't match... unget everything we grabbed
        if(out.type == Type::None)
        {
            src.back(out.tokensConsumed);
            out.tokensConsumed = 0;
        }


        return out;
    }
}