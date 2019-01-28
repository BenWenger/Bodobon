
#include <dshfs.h>
#include "driver.h"
#include "jsonfile.h"
#include "data.h"

#include "audio/nes/nes.h"

namespace bodobeep
{
    const char* const Driver::registryKey           = "bodobeep";
    const char* const Driver::regkey_channels       = "chans";
    const char* const Driver::regkey_chanuser       = "chanUser";
    const char* const Driver::regkey_songs          = "songs";
    const char* const Driver::bodkey_host           = "host";
    const char* const Driver::bodkey_curSong        = "curSong";
    const char* const Driver::bodkey_channels       = "channels";
    const char* const Driver::bodkey_songChanData   = "_chanData";


    Driver::Driver(const Host* host, const std::string& fullpath)
    {
        luawrap::LuaStackSaver stk(lua);
        
        // Loading stuffs   (the order is important!  Don't change it!)
        init_prepRegistry();
        init_loadDriverLuaFile(fullpath);
        init_buildAudioSystem();
        init_setHost(host);
    }

    void Driver::init_prepRegistry()
    {
        // Build registry basics
        lua_pushstring(lua, registryKey);
        lua_createtable(lua, 0, 3);
            lua_pushstring(lua, regkey_channels);   lua_newtable(lua);          lua_settable(lua, -3);
            lua_pushstring(lua, regkey_chanuser);   lua_newtable(lua);          lua_settable(lua, -3);
            lua_pushstring(lua, regkey_songs);      lua_newtable(lua);          lua_settable(lua, -3);
        lua_settable(lua, LUA_REGISTRYINDEX);

        // Build bodo table
        lua_createtable(lua, 0, 3);
        lua_setglobal(lua, "bodo");
    }

    void Driver::init_loadDriverLuaFile(const std::string& fullpath)
    {
        ////////////////////////////////////
        // Load the driver file!
        dshfs::Filename fn(fullpath);
        lua.loadFile(fullpath, fn.getFullName().c_str());
        lua.callFunction(0,0);
    }

    void Driver::init_buildAudioSystem()
    {
        luawrap::LuaStackSaver stk(lua);

        auto t = lua_getglobal(lua, "bodo_driver");
        if(t == LUA_TFUNCTION)
        {
            lua.callFunction(0,1);
            t = lua_type(lua,-1);
        }
        if(t != LUA_TTABLE)
            throw std::runtime_error("Lua error:  bodo_driver needs to be a table, or a function that returns a table.");

        // Pass that data to the AudioSystem factory to build our audio system
        audioSystem = std::move( AudioSystem::factory(lua) );

        // Top of the lua stack is now the channels table -- duplicate it
        dupTable();

        // one of those tables goes in registry, the other goes in the global bodo table
        pushRegistryTable();
            lua.pushString(regkey_channels);
            lua_pushvalue(lua, -3);
            lua_settable(lua, -3);
            lua_pop(lua, 2);                    // drop the registry table, AND the duplicated channel table

        // The other goes in global bodo
        lua_getglobal(lua, "bodo");
            lua.pushString(bodkey_channels);
            lua_pushvalue(lua, -3);
            lua_settable(lua, -3);
            lua_pop(lua, 2);
    }
    
    void Driver::init_setHost(const Host* host)
    {
        luawrap::LuaStackSaver stk(lua);
        lua_getglobal(lua, "bodo");
            lua.pushString(bodkey_host);
            JsonFile::pushJsonToLua(lua, host->userData);
            lua_settable(lua, -3);
            lua_pop(lua, 1);
    }

    void Driver::dupTable()
    {
        int srcIndex = lua_gettop(lua);
        lua_newtable(lua);
        int dstIndex = lua_gettop(lua);

        lua_pushnil(lua);
        while(lua_next(lua, srcIndex))
        {
            lua_pushvalue(lua, -2);     // push key
            lua_pushvalue(lua, -2);     // push value
            lua_settable(lua, dstIndex);
            lua_pop(lua, 1);            // pop key
        }
        lua_settop(lua, dstIndex);
    }

    void Driver::pushRegistryTable()
    {                                           //  bottom -> top
        lua_pushstring(lua, registryKey);       //  registryKey
        lua_gettable(lua, LUA_REGISTRYINDEX);   //  registryTbl
    }
    
    void Driver::pushRegistrySubTable(const char* tablename)
    {
        pushRegistryTable();                    //  registryTbl
        lua_pushstring(lua, tablename);         //  registryTbl,  tablename
        lua_gettable(lua, -2);                  //  registryTbl,  targetTable
        lua_remove(lua, -2);                    //  targetTable
    }

    void Driver::playSong(const Song* song)
    {
        // TODO - set bodo.cursong

        callLuaStartPlay(song);

        playStatus.clear();
        ChanPlayStatus st;
        for(auto& chan : song->channels)
        {
            st.pos = -1;
            st.lenCtr = 0;
            playStatus[chan.name] = st;
        }
        playingSong = song;

        audioSystem->play(this);
    }

    void Driver::stop()
    {
        audioSystem->stop();
    }

    timestamp_t Driver::getLengthOfTone(const Tone& tone, const std::string& chanId, int songIndex)
    {
        luawrap::LuaStackSaver stk(lua);

        if(lua_getglobal(lua, "bodo_getLength") != LUA_TFUNCTION)
            throw std::runtime_error("Lua error:  'bodo_getLength' must be a global function defined in the Lua driver");

        // param 1 = the tone
        JsonFile::pushJsonToLua(lua, tone.userData);

        // param 2 = the channel object
        lua_getglobal(lua, "bodo");
        lua_pushliteral(lua, "channels");
        lua_gettable(lua, -2);                  // get the channels table
        lua_remove(lua, -2);                    // remove 'bodo'
        lua.pushString(chanId);
        lua_gettable(lua, -2);                  // get the chan object
        lua_remove(lua, -2);                    // remove the channels table

        // param 3 = the song object
        //   TODO - do this, for now just push nil
        lua_pushnil(lua);

        // Call the function
        lua.callFunction(3, 1);

        int isnum = 0;
        auto len = static_cast<timestamp_t>(lua_tointegerx(lua, -1, &isnum));
        if(!isnum || len <= 0)
            throw std::runtime_error("Lua error:  'bodo_getLength' must return an integer greater than zero");

        return len;
    }

    void Driver::callLuaStartPlay(const Song* song)
    {
        luawrap::LuaStackSaver stk(lua);
        if(lua_getglobal(lua, "bodo_startPlay") == LUA_TFUNCTION) {
            lua_pushnil(lua);   // TODO actually push the song object
            lua.callFunction(1,0);
        }
    }
    
    bool Driver::playbackUpdate()
    {
        luawrap::LuaStackSaver stk(lua);

        try
        {
            for(auto& songCh : playingSong->channels)
            {
                //if(songCh.name != "pulse1")     continue;       // isolating for debug purposes
                auto& st = playStatus[songCh.name];
                --st.lenCtr;
                ++st.pos;

                const char* func = "bodo_updateTone";
                if(st.lenCtr <= 0)
                    func = "bodo_startTone";
                if(lua_getglobal(lua, func) != LUA_TFUNCTION)
                    throw std::runtime_error("Lua driver is missing required function " + std::string(func));

                // function is on the stack
                // push first arg (tone)
                {
                    auto& i = songCh.score.getAtTime(st.pos);
                    if(st.lenCtr <= 0)
                        st.lenCtr = i->second.length;
                    JsonFile::pushJsonToLua( lua, i->second.userData );
                }


                // push second arg (chan)
                lua_getglobal(lua, "bodo");
                lua_pushliteral(lua, "channels");
                lua_gettable(lua, -2);
                lua_remove(lua, -2);
                lua.pushString(songCh.name);
                lua_gettable(lua, -2);
                lua_remove(lua, -2);

                lua.callFunction(2,0);
            }
        }
        catch(std::exception& e)
        {
            // TODO log this somehow
            std::cout << "Error in playback:  " << e.what() << std::endl;
            return false;
        }

        return true;
    }
}
