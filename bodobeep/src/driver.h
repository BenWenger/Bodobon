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
        void            playSong(const Song* song);
        void            stop();

        timestamp_t     getLengthOfTone(const Tone& tone, const std::string& chanId, int songIndex);

    private:
        luawrap::Lua                    lua;                // should be the first item, so it is the last to be destroyed
        std::unique_ptr<AudioSystem>    audioSystem;
        std::set<std::string>           chanNames;

        struct ChanPlayStatus
        {
            timestamp_t     pos;
            timestamp_t     lenCtr;
        };
        std::map<std::string, ChanPlayStatus>       playStatus;
        const Song*                                 playingSong = nullptr;

    private:
        // Initialization stuff
        void                init_prepRegistry();
        void                init_loadDriverLuaFile(const std::string& fullpath);
        void                init_buildAudioSystem();
        void                init_setHost(const Host* host);

        // Misc
        void                dupTable();

        //Registry stuff
        void                pushRegistryTable();
        void                pushRegistrySubTable(const char* tablename);

        friend class        AudioSystem;
        void                callLuaStartPlay(const Song* song);
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