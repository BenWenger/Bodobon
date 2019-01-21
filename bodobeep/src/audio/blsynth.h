#ifndef BODOBEEP_AUDIO_BLSYNTH_H_INCLUDED
#define BODOBEEP_AUDIO_BLSYNTH_H_INCLUDED

#include "types.h"

namespace bodobeep
{
    class BlSynth
    {
    public:
        void        setFormat(int sampleRate, double clockRate, double frameSizeInClockCycles);

        void        addTransition(timestamp_t timestamp, float transition);
        int         generateSamples(const s16*& buffer, timestamp_t timestamp);

        void        clear();

    private:

        std::vector<s16>            sampleBuffer;
        std::vector<float>          transitionBuffer;
        timestamp_t                 transMult;
        timestamp_t                 transAdd;
        float                       outputSample;

        static constexpr int        fractionBits =          12;
        static constexpr int        setBits =               5;
        static constexpr int        dropBits =              fractionBits - setBits;
        static constexpr int        setMask =               (1 << setBits) - 1;
        static constexpr int        fractionMask =          (1 << fractionBits) - 1;
    };
}

#endif