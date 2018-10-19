
#include <dshfs.h>
#include <luawrap.h>
#include <iostream>

using luawrap::Lua;

int func(Lua& lua)
{
    std::cout << "In function!\n";
    return 0;
}


int main()
{
    {
        luawrap::Lua lua;
        lua.pushFunction( &func );
        lua_setglobal( lua, "callme" );

        luaL_loadstring( lua, "callme()");
        lua_pcall( lua, 0, 0, 0 );
    }

    char c;
    std::cin >> c;
}