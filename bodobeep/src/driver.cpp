
#include <dshfs.h>
#include "driver.h"
#include "jsonfile.h"

#include "audio/nes/nes.h"

namespace bodobeep
{
    Driver::Driver(const std::string& fullpath)
    {
        luawrap::LuaStackSaver stk(lua);

        lua_newtable(lua);
        lua_setglobal(lua, "bodo");

        dshfs::Filename fn(fullpath);
        lua.loadFile(fullpath, fn.getFullName().c_str());
        lua.callFunction(0,0);

        audioSystem = std::make_unique<NesAudio>();         // TODO replace this
        audioSystem->addChannelsToLua(lua);
    }

    void Driver::playSong(const Song* song)
    {
        // TODO
    }

    timestamp_t Driver::getLengthOfTone(int chanId, const Tone& tone)
    {
        luawrap::LuaStackSaver stk(lua);

        if(lua_getglobal(lua, "bodo_getLength") != LUA_TFUNCTION)
            throw std::runtime_error("Lua error:  'bodo_getLength' must be a global function defined in the Lua driver");

        lua_pushinteger(lua, chanId);
        JsonFile::pushJsonToLua(lua, tone.userData);
        lua.callFunction(2, 1);

        int isnum = 0;
        auto len = static_cast<timestamp_t>(lua_tointegerx(lua, -1, &isnum));
        if(!isnum || len <= 0)
            throw std::runtime_error("Lua error:  'bodo_getLength' must return an integer greater than zero");

        return len;
    }

    void Driver::setHostData(const json::object& hostdata)
    {
        luawrap::LuaStackSaver stk(lua);
        if(lua_getglobal(lua, "bodo") != LUA_TTABLE)
            throw std::runtime_error("Global 'bodo' table has been corrupted");

        lua_pushliteral(lua, "host");
        JsonFile::pushJsonToLua( lua, hostdata );
        lua_settable(lua, -3);
    }
}
