
#ifndef BODOBEEP_SONG_H_INCLUDED
#define BODOBEEP_SONG_H_INCLUDED

#include "types.h"
#include "tonechain.h"
#include <map>
#include <string>

namespace bodobeep
{
    struct Song
    {
        struct Channel
        {
            int             loopPos;
            ToneChain       score;
        };
        typedef std::map<std::string, Channel>      chanMap_t;

        chanMap_t           chanMap;
    };

}

#endif
