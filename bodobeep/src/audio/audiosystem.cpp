
#include "audiosystem.h"

namespace bodobeep
{

    void AudioSystem::initialize()
    {
        int samplerate = 44100;
        bool stereo = false;

        getAudioFormat(samplerate, stereo);
        sf::SoundStream::initialize(stereo ? 2 : 1, samplerate);
    }

    void AudioSystem::play()
    {
        startAudio();
        sf::SoundStream::play();
    }

    void AudioSystem::stop()
    {
        sf::SoundStream::stop();
    }

    bool AudioSystem::onGetData(sf::SoundStream::Chunk& data)
    {
        data.sampleCount = getAudio(data.samples);
        return true;
    }

    void AudioSystem::onSeek(sf::Time)
    {
        // no seeking necessary
    }
}
