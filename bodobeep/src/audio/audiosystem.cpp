
#include "audiosystem.h"

#include "nes/nes.h"
#include "driver.h"

namespace bodobeep
{
    std::unique_ptr<AudioSystem> AudioSystem::factory(luawrap::Lua& lua)
    {
        //  TODO - use the table on the top of the Lua stack to generate the appropriate AudioSystem

        auto out = std::make_unique<NesAudio>(lua);
        out->initialize();
        return std::move(out);
    }

    void AudioSystem::initialize()
    {
        int samplerate = 44100;
        bool stereo = false;

        getAudioFormat(samplerate, stereo);
        sf::SoundStream::initialize(stereo ? 2 : 1, samplerate);
    }

    void AudioSystem::play(Driver* drv)
    {
        driver = drv;
        startAudio();
        sf::SoundStream::play();
    }

    void AudioSystem::stop()
    {
        sf::SoundStream::stop();
    }

    bool AudioSystem::onGetData(sf::SoundStream::Chunk& data)
    {
        if(!driver->playbackUpdate())
            return false;
        data.sampleCount = getAudio(data.samples);
        return true;
    }

    void AudioSystem::onSeek(sf::Time)
    {
        // no seeking necessary
    }
}
