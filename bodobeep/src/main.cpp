
#include <luawrap.h>
#include "json.h"

int main()
{
    try
    {
        luawrap::Lua        lua;
        lua.loadFile("../lua/FinalFantasy.lua","FinalFantasy.lua");
    }catch(...){
        return 1;
    }

    return 0;
}