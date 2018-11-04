#ifndef BODOASM_PARSE_SYMBOL_H_INCLUDED
#define BODOASM_PARSE_SYMBOL_H_INCLUDED

#include <string>
#include "types/position.h"

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
        int             tokensConsumed = 0;

        static SymbolParse      parseDec(TokenSource& src, const std::string& topPrefix);
        static SymbolParse      parseRef(TokenSource& src, const std::string& topPrefix);
    };
}

#endif
