#ifndef BODOBEEP_AUDIO_AUDIOSYSTEM_H_INCLUDED
#define BODOBEEP_AUDIO_AUDIOSYSTEM_H_INCLUDED

#include <SFML/Audio.hpp>
#include "types.h"

#ifdef _MSC_VER
    #if defined(DEBUG) || defined (_DEBUG)
        #pragma comment(lib, "sfml-audio-d.lib")
    #else
        #pragma comment(lib, "sfml-audio.lib")
    #endif
#endif

namespace bodobeep
{

    class AudioSystem : private sf::SoundStream
    {
    public:
        virtual         ~AudioSystem() {}
        void            initialize();
        void            play();
        void            stop();
        
    protected:
        virtual void    startAudio() = 0;
        virtual int     getAudio(const s16*& buffer) = 0;
        virtual void    getAudioFormat(int& samplerate, bool& stereo) = 0;

    protected:
        // sf::SoundStream inherited members
        virtual bool    onGetData(sf::SoundStream::Chunk& data) override;
        virtual void    onSeek(sf::Time timeOffset) override;
    };
}

#endif
