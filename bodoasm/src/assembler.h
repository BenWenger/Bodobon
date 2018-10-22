#ifndef BODOASM_ASSEMBLER_H_INCLUDED
#define BODOASM_ASSEMBLER_H_INCLUDED

#include <string>
#include <luawrap.h>
using luawrap::Lua;

namespace bodoasm
{
    class Assembler
    {
    public:
        Assembler(const std::string pathToLua, const std::string pathToSrc);

    private:
        Lua         lua;

    };
}

#endif