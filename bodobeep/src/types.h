#ifndef BODOBEEP_TYPES_H_INCLUDED
#define BODOBEEP_TYPES_H_INCLUDED

#include <cstdint>
#include <map>
#include <vector>
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

    struct ToneField
    {
        enum class Type
        {
            Integer,
            String,
            IntRange
        };
        Type            type = Type::Integer;

        int             loInt = 0;
        int             hiInt = 0;
        std::string     strVal;

        ToneField() = default;
        ToneField(int v)                : type(Type::Integer),  loInt(v) {}
        ToneField(const std::string& s) : type(Type::String),   strVal(s) {}
        ToneField(int a, int b)         : type(Type::IntRange), loInt(a), hiInt(b)
        {
            if(b <= a)      type = Type::Integer;
        }
    };

    typedef std::map<std::string, std::vector<ToneField>>     ToneFieldDetails;
}

#endif