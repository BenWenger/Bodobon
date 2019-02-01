#ifndef BODOBEEP_AUDIO_NES_NES_H_INCLUDED
#define BODOBEEP_AUDIO_NES_NES_H_INCLUDED

#include <memory>
#include "audio/audiosystem.h"
#include "audio/blsynth.h"

namespace bodobeep
{
    class NesChannel;

    class NesAudio : public AudioSystem
    {
    public:
                        NesAudio();
        virtual         ~NesAudio();
        
        virtual void    resetAudio() override;
        
        virtual void    stopChannel(const std::string& chanName) override;
        
    protected:
        virtual int     getAudio(const s16*& buffer) override;
        virtual void    getAudioFormat(int& samplerate, bool& stereo) override;

        
        virtual std::vector<std::string>    createChannels(luawrap::Lua& lua, const DriverSpec& spec) override;

    private:
        std::vector<NesChannel*>            channels;
        BlSynth                             synth;

        static constexpr timestamp_t        frameCycs = static_cast<timestamp_t>(((341.0 * 261.0) + 340.5) / 3.0);
        static constexpr double             cpuClockRate = 1789772.7272;
        static constexpr int                sampleRate = 48000;
    };
}

#endif 