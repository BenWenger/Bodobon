#ifndef BODOBEEP_AUDIO_NES_NESCHANNEL_H_INCLUDED
#define BODOBEEP_AUDIO_NES_NESCHANNEL_H_INCLUDED

#include "types.h"
#include <luawrap.h>

namespace bodobeep
{
    class BlSynth;
    class NesChannel : public luawrap::LuaObject
    {
    public:
                        NesChannel(const std::string& _name) : name(_name) {}
        virtual         ~NesChannel() {}
        
        virtual void    runForCycs(BlSynth& synth, timestamp_t cycs) = 0;
        virtual void    reset() = 0;
        const std::string           name;

    protected:
        static const float          totalBase;
        static const float          pulseBase;
        static const float          triangleBase;
        static const float          noiseBase;
        static const float          dmcBase;
    };
}

#endif
