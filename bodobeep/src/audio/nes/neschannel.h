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
        static constexpr float      totalBase   = 2.0f;
        static constexpr float      pulseBase   = 0.00752f * 0.5f;
        static constexpr float      triangleBase= 0.00851f * 2;
        static constexpr float      noiseBase   = 0.00494f;
        static constexpr float      dmcBase     = 0.00335f;
    };
}

#endif
