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
        luawrap::Lua                    lua;
        std::unique_ptr<AudioSystem>    audioSystem;
        std::set<std::string>           chanNames;

        struct ChanPlayStatus
        {
            timestamp_t     pos;
            timestamp_t     lenCtr;
        };
        std::map<std::string, ChanPlayStatus>       playStatus;
        const Song*                                 playingSong;

    private:
        friend class        AudioSystem;
        void                callLuaStartPlay(const Song* song);
        bool                playbackUpdate();
    };

}

#endif