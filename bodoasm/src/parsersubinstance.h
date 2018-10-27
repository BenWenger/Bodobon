#ifndef BODOASM_PARSERSUBINSTANCE_H_INCLUDED
#define BODOASM_PARSERSUBINSTANCE_H_INCLUDED

#include <string>
#include <vector>
#include "token.h"
#include "error.h"

namespace bodoasm
{
    class ParserSubInstance
    {
    public:
        ParserSubInstance(ErrorReporter& er, bool muteerr, const Token* t, size_t siz, size_t start = 0);
    private:
        ErrorReporter&          err;

        const Token*            tokens;
        size_t                  arSize;
        size_t                  curPos;
        bool                    mute;
    };
}

#endif