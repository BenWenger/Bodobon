#ifndef BODOASM_PARSE_SYMBOL_H_INCLUDED
#define BODOASM_PARSE_SYMBOL_H_INCLUDED

#include <string>
#include <vector>
#include "types/position.h"
#include "types/token.h"

namespace bodoasm
{
    class   TokenSource;

    struct SymbolParse
    {
        enum class Type { None, Local, Top, Full };
        Type            type = Type::None;
        std::string     name;
        Position        pos;
        bool            ws_after;

        static SymbolParse      parseDec(TokenSource& src, const std::string& topPrefix);
        static SymbolParse      parseRef(TokenSource& src, const std::string& topPrefix);

        void            unget(TokenSource& src);

        operator bool () const      { return type != Type::None;        }
        bool operator ! () const    { return type == Type::None;        }

    private:
        std::vector<Token>      tokens;
    };
}

#endif
