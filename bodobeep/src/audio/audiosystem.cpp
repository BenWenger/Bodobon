
#include "audiosystem.h"

#include "nes/nes.h"
#include "driver.h"

namespace bodobeep
{
    std::unique_ptr<AudioSystem> AudioSystem::factory(luawrap::Lua& lua)
    {
        auto spec = getDriverSpecFromLua(lua);                              // pull the driver spec from the lua
        lua_pop(lua, 1);                                                    // pop that driver spec
        lua_createtable(lua, 0, static_cast<int>(spec.channels.size()));    // replace with table to hold channel data

        std::unique_ptr<AudioSystem>        out;

        if(spec.systemName == "nes")        out = std::make_unique<NesAudio>();
        // ADD OTHER SPECS HERE

        if(!out)
            throw std::runtime_error("Error when loading driver.  Audio system \"" + spec.systemName + "\" is not recognized or supported");

        out->chanNames = out->createChannels(lua, spec);
        out->primeAudio();
        return std::move(out);
    }

    void AudioSystem::primeAudio()
    {
        int samplerate = 48000;
        bool stereo = false;

        getAudioFormat(samplerate, stereo);
        sf::SoundStream::initialize(stereo ? 2 : 1, samplerate);
    }

    void AudioSystem::play(Driver* drv)
    {
        driver = drv;
        sf::SoundStream::play();
    }

    void AudioSystem::stop()
    {
        sf::SoundStream::stop();
    }

    bool AudioSystem::onGetData(sf::SoundStream::Chunk& data)
    {
        if(!driver->playbackUpdate())
            return false;
        data.sampleCount = getAudio(data.samples);
        return true;
    }

    void AudioSystem::onSeek(sf::Time)
    {
        // no seeking necessary
    }

    auto AudioSystem::getDriverSpecFromLua(luawrap::Lua& lua) -> DriverSpec
    {
        if(lua_type(lua, -1) != LUA_TTABLE)
            throw std::runtime_error("Internal error:  AudioSystem::getDriverSpecFromLua expects a table on top of the Lua stack");

        luawrap::LuaStackSaver stk(lua);
        DriverSpec out;

        bool foundSystem = false;
        bool foundChannels = false;
        lua_pushnil(lua);
        while(lua_next(lua, -2))
        {
            auto key = lua.toString(-2);
            if(key == "system")
            {
                foundSystem = true;
                bool sysName = false;
                if(lua_type(lua, -1) != LUA_TTABLE)     throw std::runtime_error("Lua error:  bodo_driver's \"system\" value must be a table");
                for(int i = 0; i < 1000; ++i)       // 1000 is arbitrary -- this probably could be a while loop.
                {
                    auto t = lua_geti(lua, -1, i);
                    if(t == LUA_TNIL) {
                        lua_pop(lua,1);
                        if(i == 0)      continue;
                        else            break;
                    }
                    if(t != LUA_TSTRING)                throw std::runtime_error("Lua error:  bodo_driver's \"system\" table must have string values");
                    if(sysName)             out.systemExtra.push_back( lua.toString(-1,false) );
                    else                    out.systemName = lua.toString(-1,false);
                    sysName = true;
                    lua_pop(lua, 1);
                }
            }
            else if(key == "channels")
            {
                foundChannels = true;
                if(lua_type(lua, -1) != LUA_TTABLE)     throw std::runtime_error("Lua error:  bodo_driver's \"channels\" value must be a table");
                for(int i = 0; i < 1000; ++i)       // 1000 is arbitrary -- this probably could be a while loop.
                {
                    auto t = lua_geti(lua, -1, i);
                    if(t == LUA_TNIL) {
                        lua_pop(lua,1);
                        if(i == 0)      continue;
                        else            break;
                    }
                    if(t != LUA_TSTRING)                throw std::runtime_error("Lua error:  bodo_driver's \"channels\" table must have string values");
                    out.channels.push_back( lua.toString(-1,false) );
                    lua_pop(lua, 1);
                }
            }
            lua_pop(lua, 1);        // drop the value for next loop
        }
        
        if(!foundSystem)        throw std::runtime_error("Lua error:  bodo_driver is missing \"system\" entry");
        if(!foundChannels)      throw std::runtime_error("Lua error:  bodo_driver is missing \"channels\" entry");

        return out;
    }
}
