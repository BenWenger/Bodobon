#ifndef BODOBEEP_TYPES_H_INCLUDED
#define BODOBEEP_TYPES_H_INCLUDED

#include <cstdint>
#include "json.h"

namespace bodobeep
{
    typedef std::int16_t        s16;
    typedef std::int32_t        s32;
    typedef std::int_fast32_t   timestamp_t;

    struct Tone
    {
        timestamp_t         length;
        json::value         userData;
    };

    struct PitchRange
    {
        int                 range = 1;
        bool                hasRest = false;
        bool                hasSustain = false;
    };

    enum PitchPos
    {
        Rest =      -1,
        Sustain =   -2,
        Unknown =   -3
    };
}

#endif