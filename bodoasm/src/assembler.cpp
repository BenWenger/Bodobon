
#include "assembler.h"

namespace bodoasm
{

    Assembler::Assembler(const std::string pathToLua, const std::string pathToSrc)
    {
        luaL_openlibs(lua);
    }

}
