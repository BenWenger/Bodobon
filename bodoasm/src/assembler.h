#ifndef BODOASM_ASSEMBLER_H_INCLUDED
#define BODOASM_ASSEMBLER_H_INCLUDED

#include <string>
#include <luawrap.h>
#include "error.h"
#include "lexer.h"
using luawrap::Lua;

namespace bodoasm
{
    class Assembler
    {
    public:
                            Assembler(const std::string& pathToLua);
        bool                doFile(const std::string& path);
        bool                finalizeAndOutput(const std::string& path);

    private:
        Lua                 lua;
        ErrorReporter       err;
        Lexer               lexer;
    };
}

#endif