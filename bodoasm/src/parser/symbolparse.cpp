
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
        out.pos = t.pos;
        // local?
        if(t.str == "." && !t.ws_after)
        {
            auto name = src.next();
            if(name.isPossibleSymbol())
            {
                out.tokens.push_back(t);
                out.tokens.push_back(name);
                out.name = topPrefix + "." + name.str;
                out.type = Type::Local;
                out.ws_after = name.ws_after;
            }
            else
            {
                src.unget(name);
                src.unget(t);
            }
        }
        else if(t.isPossibleSymbol())
        {
            out.tokens.push_back(t);
            out.name = t.str;
            out.type = Type::Top;
            out.ws_after = t.ws_after;
        }
        else
            src.unget(t);

        return out;
    }

    SymbolParse SymbolParse::parseRef(TokenSource& src, const std::string& topPrefix)
    {
        // References can be top level:             sym
        // Or local:                                .sym
        // Or fully qualified:                      sym.sym
        SymbolParse out;
        
        auto a = src.next();
        auto b = src.next();
        auto c = src.next();
        out.pos =               a.pos;

        if(a.str == "." && !a.ws_after)
        {
            if(b.isPossibleSymbol())
            {
                out.name = topPrefix + "." + b.str;
                out.type = Type::Local;
                out.ws_after = b.ws_after;
            }
        }
        else if(a.isPossibleSymbol())
        {
            if(a.ws_after)      // if there's whitespace after, there's not a dot
            {
                out.name = a.str;
                out.type = Type::Top;
                out.ws_after = true;
            }
            else
            {
                if(b.str == "." && !b.ws_after)
                {
                    if(c.isPossibleSymbol())
                    {
                        out.type = Type::Full;
                        out.ws_after = c.ws_after;
                        out.name = a.str + "." + c.str;
                    }
                }
                else  // no dot, just top level
                {
                    out.name = a.str;
                    out.type = Type::Top;
                    out.ws_after = false;
                }
            }
        }

        // Unget whatever didn't match, keep what did
        switch(out.type)
        {
        case Type::None:    src.unget(c);               src.unget(b);               src.unget(a);               break;
        case Type::Top:     out.tokens.push_back(a);    src.unget(c);               src.unget(b);               break;
        case Type::Local:   out.tokens.push_back(a);    out.tokens.push_back(b);    src.unget(c);               break;
        case Type::Full:    out.tokens.push_back(a);    out.tokens.push_back(b);    out.tokens.push_back(c);    break;
        }
        return out;
    }

    void SymbolParse::unget(TokenSource& src)
    {
        while(!tokens.empty())
        {
            src.unget(tokens.back());
            tokens.pop_back();
        }
    }
}