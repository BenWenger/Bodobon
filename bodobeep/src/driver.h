#ifndef BODOBEEP_DRIVER_H_INCLUDED
#define BODOBEEP_DRIVER_H_INCLUDED

#include <thread>
#include <string>
#include <memory>
#include <luawrap.h>
#include "audio/audiosystem.h"

namespace bodobeep
{
    class Song;
    class Driver
    {
    public:
                        Driver() = delete;
                        Driver(const std::string& fullpath);
        void            playSong(const Song* song);

        void            setHostData(const json::object& hostdata);

        timestamp_t     getLengthOfTone(int chanId, const Tone& tone);

    private:
        luawrap::Lua                    lua;
        std::unique_ptr<AudioSystem>    audioSystem;
    };

}

#endif