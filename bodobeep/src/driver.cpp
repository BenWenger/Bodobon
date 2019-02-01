
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
        init_prepChannelUserData();
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
        pushBodoTable();
            lua.pushString(bodkey_host);
            JsonFile::pushJsonToLua(lua, host->userData);
            lua_settable(lua, -3);
            lua_pop(lua, 1);
    }

    void Driver::init_prepChannelUserData()
    {
        luawrap::LuaStackSaver stk(lua);
        for(auto& i : audioSystem->getChanNames())
        {
            lua_newtable(lua);
            popAndSetChannelUserData(lua, i);
        }
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

    void Driver::pushBodoTable()
    {
        if(lua_getglobal(lua, "bodo") != LUA_TTABLE)
        {
            lua_pop(lua, 1);
            throw std::runtime_error("Lua error:  Globally accessed 'bodo' table has been corrupted.");
        }
    }

    void Driver::pushRegistryTable(luawrap::Lua& lua)
    {                                           //  bottom -> top
        lua_pushstring(lua, registryKey);       //  registryKey
        lua_gettable(lua, LUA_REGISTRYINDEX);   //  registryTbl
    }
    
    void Driver::pushRegistrySubTable(luawrap::Lua& lua, const char* tablename)
    {
        pushRegistryTable(lua);                 //  registryTbl
        lua_pushstring(lua, tablename);         //  registryTbl,  tablename
        lua_gettable(lua, -2);                  //  registryTbl,  targetTable
        lua_remove(lua, -2);                    //  targetTable
    }

    void Driver::registerSong(Song* song)
    {
        luawrap::LuaStackSaver stk(lua);

        pushRegistrySubTable(regkey_songs);                         // songs
        lua_pushlightuserdata(lua, reinterpret_cast<void*>(song));  // songs, songKey
        JsonFile::pushJsonToLua(lua, song->userData);               // songs, songKey, songTable
        lua.pushString(bodkey_songChanData);                        // songs, songKey, songTable, "_chanData"
        lua_newtable(lua);                                          // songs, songKey, songTable, "_chanData", dataTable

        // push channel user data
        for(auto& i : song->channels)
        {
            lua.pushString(i.name);                                 // ..., dataTable, chanName
            JsonFile::pushJsonToLua(lua, i.userData);               // ..., dataTable, chanName, chanUserData
            lua_settable(lua, -3);                                  // ..., dataTable
        }

        // at this point:                                           // songs, songKey, songTable, "_chanData", dataTable
        lua_settable(lua, -3);                                      // songs, songKey, songTable
        lua_settable(lua, -3);                                      // songs
        lua_pop(lua, 1);                                            // [empty]
    }

    void Driver::pushSong(Song* song)
    {
        pushRegistrySubTable(regkey_songs);                         // songs
        lua_pushlightuserdata(lua, reinterpret_cast<void*>(song));  // songs, songKey
        auto t = lua_gettable(lua, -2);                             // songs, songTable
        lua_remove(lua, -2);                                        // songTable

        if(t != LUA_TTABLE)
            throw std::runtime_error("Internal error in Driver:  pushSong attempting to push a song that hasn't been registered");
    }
    
    void Driver::pushTone(const Tone& tone)
    {
        //  Pretty simple!
        JsonFile::pushJsonToLua(lua, tone.userData);
    }

    void Driver::pushChannel(const std::string& chanName)
    {
        pushRegistrySubTable(regkey_channels);      // channels
        lua.pushString(chanName);                   // channels, key
        auto t = lua_gettable(lua, -2);             // channels, goal
        lua_remove(lua, -2);                        // goal

        if(t != LUA_TUSERDATA)
            throw std::runtime_error("Internal error in Driver:  pushChannel found entry in channels registry that was not a Channel object");
    }

    void Driver::setCurSong(Song* song)
    {
        luawrap::LuaStackSaver stk(lua);
        pushBodoTable();
        lua.pushString(bodkey_curSong);
        pushSong(song);
        lua_settable(lua, -3);
    }
    
    void Driver::pushChannelUserData(luawrap::Lua& lua, const std::string& chanName)
    {
        pushRegistrySubTable(lua, regkey_chanuser);         // chantable
        lua.pushString(chanName);                           // chantable, key
        lua_gettable(lua, -2);                              // chantable, goal
        lua_remove(lua, -2);                                // goal
    }

    void Driver::popAndSetChannelUserData(luawrap::Lua& lua, const std::string& chanName)
    {                                                       // value                            <- stack starts with this on it
        pushRegistrySubTable(lua, regkey_chanuser);         // value, chantable
        lua.pushString(chanName);                           // value, chantable, key
        lua_pushvalue(lua, -3);                             // value, chantable, key, value
        lua_settable(lua, -3);                              // value, chantable
        lua_pop(lua, 2);                                    // [empty]
    }

    void Driver::playSong(Song* song)
    {
        setCurSong(song);
        
        playStatus.clear();
        ChanPlayStatus st;
        for(auto& chan : song->channels)
        {
            st.pos = -1;
            st.lenCtr = 0;
            st.endReached = false;
            playStatus[chan.name] = st;
        }
        
        audioSystem->resetAudio();

        callLuaStartPlay(song);
        playingSong = song;

        audioSystem->play(this);
    }

    void Driver::stop()
    {
        audioSystem->stop();
    }

    timestamp_t Driver::getLengthOfTone(const Tone& tone, const std::string& chanId, Song* song)
    {
        luawrap::LuaStackSaver stk(lua);

        if(lua_getglobal(lua, "bodo_getLength") != LUA_TFUNCTION)
            throw std::runtime_error("Lua error:  'bodo_getLength' must be a global function defined in the Lua driver");

        pushTone(tone);         // param 1
        pushChannel(chanId);    // param 2
        pushSong(song);         // param 3

        // Call the function
        lua.callFunction(3, 1);

        int isnum = 0;
        auto len = static_cast<timestamp_t>(lua_tointegerx(lua, -1, &isnum));
        if(!isnum || len <= 0)
            throw std::runtime_error("Lua error:  'bodo_getLength' must return an integer greater than zero");

        return len;
    }

    void Driver::callLuaStartPlay(Song* song)
    {
        luawrap::LuaStackSaver stk(lua);
        if(lua_getglobal(lua, "bodo_startPlay") == LUA_TFUNCTION) {
            pushSong(song);         // param 1
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
                auto& st = playStatus[songCh.name];
                if(st.endReached)
                    continue;

                bool newtone = false;
                --st.lenCtr;
                ++st.pos;

                // Do we need to get a new tone?
                if(st.lenCtr <= 0)
                {
                    newtone = true;
                    auto i = songCh.score.getAtTime(st.pos);
                    if(i == songCh.score.end())     // end of song reached
                    {
                        if(songCh.loopPos >= 0)
                            i = songCh.score.getNth(songCh.loopPos);
                        if(i == songCh.score.end())
                        {
                            st.endReached = true;
                            audioSystem->stopChannel(songCh.name);
                            continue;
                        }
                        else
                            st.pos = i->first;
                    }
                    st.playingTone = i->second;
                    st.lenCtr = st.playingTone.length;
                }

                // push function
                const char* func = newtone ? "bodo_startTone" : "bodo_updateTone";
                if(lua_getglobal(lua, func) != LUA_TFUNCTION)
                    throw std::runtime_error("Lua driver is missing required function " + std::string(func));

                pushTone(st.playingTone);       // param 1
                pushChannel(songCh.name);       // param 2

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
