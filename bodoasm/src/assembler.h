#ifndef BODOASM_ASSEMBLER_H_INCLUDED
#define BODOASM_ASSEMBLER_H_INCLUDED

#include <string>
#include <luawrap.h>
#include "srcfilemanager.h"
#include "error.h"
using luawrap::Lua;

namespace bodoasm
{
    class Assembler
    {
    public:
        Assembler(const std::string pathToLua, const std::string pathToSrc);

    private:
        Lua                 lua;
        ErrorReporter       err;
        SrcFileManager      srcFileMgr;
    };
}

#endif