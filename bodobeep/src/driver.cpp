
#include <dshfs.h>
#include "driver.h"
#include "jsonfile.h"
#include "data.h"

#include "audio/nes/nes.h"

namespace bodobeep
{
    Driver::Driver(const Host* host, const std::string& fullpath)
    {
        // First, put the super-secret private table on the stack
        lua_createtable(lua, 0, 1);     // at position '1', always

        // Stack saver from here (don't want it to drop our private table -- EVER)
        luawrap::LuaStackSaver stk(lua);

        // finish building the secret table
        lua_pushliteral(lua, "chanUser");
        lua_newtable(lua);
        lua_settable(lua, -3);

        // Now actually load the file!
        dshfs::Filename fn(fullpath);
        lua.loadFile(fullpath, fn.getFullName().c_str());
        lua.callFunction(0,0);

        // Now that we have the Lua file, call the bodo_driver func to get our audio system
        {
            auto t = lua_getglobal(lua, "bodo_driver");
            if(t == LUA_TFUNCTION)
            {
                lua.callFunction(0,1);
                t = lua_type(lua,-1);
            }
            if(t != LUA_TTABLE)
                throw std::runtime_error("Lua error:  bodo_driver needs to be a table, or a function that returns a table.");
        }

        // Build the audio system
        audioSystem = std::move( AudioSystem::factory(lua) );

        // Make global 'bodo' table
        lua_createtable(lua, 0, 4);     // 4 entries:  'host', 'curSong', 'channels', 'songs'
        {
            // Set 'bodo.host'
            lua_pushliteral(lua, "host");
            JsonFile::pushJsonToLua( lua, host->userData );
            lua_settable(lua, -3);

            // Set 'bodo.channels'
            lua_pushliteral(lua, "channels");
            lua_newtable(lua);
            {
                luawrap::LuaStackSaver wooooop(lua);
                chanNames = audioSystem->addChannelsToLua(lua);

                {
                    // create 'chanUser' in our super secret table
                    //   none of this actually touches the "channels" table, but it's put here for convenience
                    lua_pushliteral(lua, "chanUser");
                    lua_createtable(lua, 0, static_cast<int>(chanNames.size()));
                    for(auto& n : chanNames)
                    {
                        lua.pushString(n);
                        lua_newtable(lua);
                        lua_settable(lua, -3);
                    }
                    lua_settable(lua, 1);       // super sekrit
                }
            }
            lua_settable(lua, -3);

            // 'songs' is just an empty table for now
            lua_pushliteral(lua, "songs");
            lua_newtable(lua);
            lua_settable(lua, -3);
        }
        // Set global 'bodo' table
        lua_setglobal(lua, "bodo");


        // Once 'bodo' is prepped, we can call bodo_init to notify
        auto t = lua_getglobal(lua, "bodo_init");
        if(t == LUA_TFUNCTION)
            lua.callFunction(0,0);
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
