
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
    
    const std::string Driver::luausr_driver                 = "bodo_driver";
    const std::string Driver::luausr_init                   = "bodo_init";
    const std::string Driver::luausr_getLength              = "bodo_getLength";
    const std::string Driver::luausr_playStartSong          = "bodo_playStartSong";
    const std::string Driver::luausr_playStartTone          = "bodo_playStartTone";
    const std::string Driver::luausr_playUpdateTone         = "bodo_playUpdateTone";
    const std::string Driver::luausr_getPitchRange          = "bodo_getPitchRange";
    const std::string Driver::luausr_getPitchPos            = "bodo_getPitchPos";
    const std::string Driver::luausr_setTonePitch           = "bodo_setTonePitch";
    const std::string Driver::luausr_getToneEditDetails     = "bodo_getToneEditDetails";


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

        auto t = lua_getglobal(lua, Driver::luausr_driver.c_str());
        if(t == LUA_TFUNCTION)
        {
            lua.callFunction(0,1);
            t = lua_type(lua,-1);
        }
        if(t != LUA_TTABLE)
            throw std::runtime_error("Lua error:  " + Driver::luausr_driver + " needs to be a table, or a function that returns a table.");

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

        if(lua_getglobal(lua, Driver::luausr_getLength.c_str()) != LUA_TFUNCTION)
        {
            // If there isn't a function, look at the 'length' property of the tone, if it exists
            if(tone.userData.is<json::object>())
            {
                auto& obj = tone.userData.get<json::object>();
                auto i = obj.find("length");
                if(i != obj.end())
                {
                    auto& len = i->second;
                    if(len.is<std::int64_t>())
                    {
                        auto out = static_cast<timestamp_t>(len.get<std::int64_t>());
                        if(out > 0)
                            return out;
                    }
                }
            }
            throw std::runtime_error("Lua error:  '" + Driver::luausr_getLength + "' is not a global function defined in the Lua driver, and a tone is missing a valid 'length' property");
        }

        pushTone(tone);         // param 1
        pushChannel(chanId);    // param 2
        pushSong(song);         // param 3

        // Call the function
        lua.callFunction(3, 1);

        int isnum = 0;
        auto len = static_cast<timestamp_t>(lua_tointegerx(lua, -1, &isnum));
        if(!isnum || len <= 0)
            throw std::runtime_error("Lua error:  '" + Driver::luausr_getLength + "' must return an integer greater than zero");

        return len;
    }

    void Driver::callLuaStartPlay(Song* song)
    {
        luawrap::LuaStackSaver stk(lua);
        if(lua_getglobal(lua, luausr_playStartSong.c_str()) == LUA_TFUNCTION) {
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
                const char* func = newtone ? luausr_playStartTone.c_str() : luausr_playUpdateTone.c_str();
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


    PitchRange Driver::getPitchRange(const std::string& chanName, Song* song)
    {
        luawrap::LuaStackSaver stk(lua);

        PitchRange out;

        if(lua_getglobal(lua, luausr_getPitchRange.c_str()) != LUA_TFUNCTION)
            throw std::runtime_error("Lua driver is missing required function " + luausr_getPitchRange);

        pushChannel(chanName);      // param 1
        pushSong(song);             // param 2
        int resultCount = lua.callFunction(2, -1);

        if(resultCount <= 0)
            throw std::runtime_error("Lua driver error:  " + luausr_getPitchRange + " must return at least one value");

        int isnum = 0;
        out.range = static_cast<int>(lua_tointegerx(lua, -resultCount, &isnum));
        if(!isnum)
            throw std::runtime_error("Lua driver error:  " + luausr_getPitchRange + "'s first return value must be an integer");
        if(out.range <= 0)
            throw std::runtime_error("Lua driver error:  Pitch range provided by " + luausr_getPitchRange + " must be greater than zero");

        // look for other things
        for(int i = -resultCount + 1; i < 0; ++i)
        {
            if(lua_type(lua, i) != LUA_TSTRING)     throw std::runtime_error("Lua driver error:  " + luausr_getPitchRange + " return values after the first must be strings");
            auto str = lua.toString(i,false);
            if(str == "rest")           out.hasRest = true;
            else if(str == "sustain")   out.hasSustain = true;
        }

        return out;
    }

    int Driver::getPitchPos(const Tone& tone, const std::string& chanName, Song* song)
    {
        luawrap::LuaStackSaver stk(lua);

        if(lua_getglobal(lua, luausr_getPitchPos.c_str()) != LUA_TFUNCTION)
            return getPitchPos_FromToneData(tone);

        pushTone(tone);
        pushChannel(chanName);
        pushSong(song);
        lua.callFunction(3, 1);

        int out = PitchPos::Unknown;

        switch(lua_type(lua, -1))
        {
        case LUA_TNUMBER:
            {
                int isnum = 0;
                out = static_cast<int>( lua_tointegerx(lua, -1, &isnum) );
                if(out < 0 || !isnum)
                    out = PitchPos::Unknown;
            }break;
        case LUA_TSTRING:
            {
                auto str = lua.toString(-1, false);
                if(str == "rest")           out = PitchPos::Rest;
                else if(str == "sustain")   out = PitchPos::Sustain;
                else                        out = PitchPos::Unknown;
            }break;
        default:
            throw std::runtime_error("Lua driver error:  " + luausr_getPitchPos + " returned an invalid type");
        }

        return out;
    }
    
    int Driver::getPitchPos_FromToneData(const Tone& tone)
    {
        if(!tone.userData.is<json::object>())
            throw std::runtime_error(luausr_getPitchPos + " is missing from driver Lua, and pitch cannot be determined from the tone data because it's not an object");

        auto& obj = tone.userData.get<json::object>();
        auto i = obj.find("pitch");
        if(i == obj.end())
            throw std::runtime_error(luausr_getPitchPos + " is missing from driver Lua, and the tone data lacks a 'pitch' property");

        int out = PitchPos::Unknown;
        auto& v = i->second;
        if(v.is<std::int64_t>())
        {
            out = static_cast<int>( v.get<std::int64_t>() );
            if(out < 0)     out = PitchPos::Unknown;
        }
        else if(v.is<std::string>())
        {
            auto& str = v.get<std::string>();
            if(str == "rest")           out = PitchPos::Rest;
            else if(str == "sustain")   out = PitchPos::Sustain;
        }

        return out;
    }

    void Driver::setTonePitch(int pitchPos, Tone& tone, const std::string& chanName, Song* song)
    {
        if(pitchPos == PitchPos::Unknown)       // shouldn't happen, but if it does, it should be a NOP
            return;

        luawrap::LuaStackSaver stk(lua);

        if(lua_getglobal(lua, luausr_setTonePitch.c_str()) != LUA_TFUNCTION)
        {
            setTonePitch_NoLua(pitchPos, tone);
            return;
        }
        
        if(pitchPos == PitchPos::Rest)          lua_pushliteral(lua, "rest");
        else if(pitchPos == PitchPos::Sustain)  lua_pushliteral(lua, "sustain");
        else                                    lua_pushinteger(lua, static_cast<lua_Integer>(pitchPos));

        JsonFile::pushJsonToLua(lua, tone.userData);
        pushChannel(chanName);
        pushSong(song);
        lua.callFunction(4, 1);
        tone.userData = JsonFile::getJsonFromLua(lua, -1);
    }

    void Driver::setTonePitch_NoLua(int pitchPos, Tone& tone)
    {
        if(!tone.userData.is<json::object>())
            throw std::runtime_error(luausr_setTonePitch + " is missing from driver Lua, and pitch cannot be determined from the tone data because it's not an object");
        
        auto& obj = tone.userData.get<json::object>();
        
        if(pitchPos >= 0)                       obj["pitch"] = json::value( static_cast<std::int64_t>(pitchPos) );
        else if(pitchPos == PitchPos::Rest)     obj["pitch"] = json::value("rest");
        else if(pitchPos == PitchPos::Sustain)  obj["pitch"] = json::value("sustain");
    }
}
