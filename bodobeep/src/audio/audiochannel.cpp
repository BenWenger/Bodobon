
#include "driver.h"
#include "audiochannel.h"

namespace bodobeep
{
    void AudioChannel::addLuaMetaMethods(luawrap::Lua& lua)
    {
        lua_pushliteral(lua, "__index");
        lua.pushFunction(this, &AudioChannel::lua_index);
        lua_settable(lua, -3);
        
        lua_pushliteral(lua, "__newindex");
        lua.pushFunction(this, &AudioChannel::lua_newIndex);
        lua_settable(lua, -3);

    }

    int AudioChannel::lua_index(luawrap::Lua& lua)
    {
        if(!lua_isstring(lua, -1))
            return 0;

        auto index = lua.toString(-1);
        if(index == "name")
        {
            lua.pushString(name);
            return 1;
        }
        if(index == "data")
        {
            Driver::pushChannelUserData(lua, name);
            return 1;
        }

        auto oldtop = lua_gettop(lua);
        pushMember(lua, index);
        auto newtop = lua_gettop(lua);

        if(newtop <= oldtop)
            lua_pushnil(lua);
        return 1;
    }

    int AudioChannel::lua_newIndex(luawrap::Lua& lua)
    {
        if(!lua_isstring(lua, -2))
            return 0;

        // The only thing that should be able to be set by the user is the "data" table
        if(lua.toString(-2) == "data")
            Driver::popAndSetChannelUserData(lua, name);

        return 0;
    }
}