#ifndef BODOBEEP_DRIVER_H_INCLUDED
#define BODOBEEP_DRIVER_H_INCLUDED

#include <thread>
#include <string>
#include <memory>
#include <luawrap.h>
#include "audio/audiosystem.h"

namespace bodobeep
{
    class Score;

    class Driver
    {
    public:
        void            loadDriverFile(const std::string& filename);
        void            playScore(const Score* score);

    private:
        luawrap::Lua                    lua;
        std::unique_ptr<AudioSystem>    audioSystem;
    };

}

#endif