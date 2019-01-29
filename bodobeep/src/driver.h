#ifndef BODOBEEP_DRIVER_H_INCLUDED
#define BODOBEEP_DRIVER_H_INCLUDED

#include <thread>
#include <string>
#include <memory>
#include <luawrap.h>
#include <set>
#include <map>
#include <vector>
#include "audio/audiosystem.h"

namespace bodobeep
{
    class Song;
    class Host;
    class Driver
    {
    public:
                        Driver() = delete;
                        Driver(const Host* host, const std::string& fullpath);
        void            playSong(Song* song);
        void            stop();

        void            registerSong(Song* song);

        timestamp_t     getLengthOfTone(const Tone& tone, const std::string& chanName, Song* song);

        static void     pushChannelUserData(luawrap::Lua& lua, const std::string& chanName);
        static void     popAndSetChannelUserData(luawrap::Lua& lua, const std::string& chanName);

    private:
        luawrap::Lua                    lua;                // should be the first item, so it is the last to be destroyed
        std::unique_ptr<AudioSystem>    audioSystem;

        struct ChanPlayStatus
        {
            timestamp_t     pos;
            timestamp_t     lenCtr;
        };
        std::map<std::string, ChanPlayStatus>       playStatus;
        Song*                                       playingSong = nullptr;

    private:
        // Initialization stuff
        void                init_prepRegistry();
        void                init_loadDriverLuaFile(const std::string& fullpath);
        void                init_buildAudioSystem();
        void                init_setHost(const Host* host);
        void                init_prepChannelUserData();


        //Registry and support stuff
        void                dupTable();
        void                pushRegistryTable()                             { pushRegistryTable(lua);                   }
        void                pushRegistrySubTable(const char* tablename)     { pushRegistrySubTable(lua, tablename);     }
        static void         pushRegistryTable(luawrap::Lua& lua);
        static void         pushRegistrySubTable(luawrap::Lua& lua, const char* tablename);
        void                pushBodoTable();
        void                setCurSong(Song* song);         // (song must be registered)
        
        void                pushSong(Song* song);           // (song must be registered)
        void                pushTone(const Tone& tone);
        void                pushChannel(const std::string& chanName);

        friend class        AudioSystem;
        void                callLuaStartPlay(Song* song);
        bool                playbackUpdate();

        //////////////////////////////////////////
        //  Constant keys
                // stuff in the registry
        static const char* const        registryKey;
        static const char* const        regkey_channels;        // channel list
        static const char* const        regkey_chanuser;        // user modifiable data for each channel (user accessible only through channel objects)
        static const char* const        regkey_songs;           // loaded song list
                // stuff in user-accessible global "bodo" table
        static const char* const        bodkey_host;            // host user data
        static const char* const        bodkey_curSong;         // current song
        static const char* const        bodkey_channels;        // channel list (user accessible)
        static const char* const        bodkey_songChanData;    // user data for each channel in a song
    };

}

#endif