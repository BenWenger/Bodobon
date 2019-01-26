#ifndef BODOBEEP_AUDIO_AUDIOSYSTEM_H_INCLUDED
#define BODOBEEP_AUDIO_AUDIOSYSTEM_H_INCLUDED

#include <SFML/Audio.hpp>
#include <luawrap.h>
#include <memory>
#include <set>
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
    class Driver;

    class AudioSystem : private sf::SoundStream
    {
    public:
        virtual         ~AudioSystem() {}
        void            play(Driver* drv);
        void            stop();

        virtual std::set<std::string>           addChannelsToLua(luawrap::Lua& lua) = 0;

        static std::unique_ptr<AudioSystem>     factory(luawrap::Lua& lua);
        
    protected:
        virtual void    startAudio() = 0;
        virtual int     getAudio(const s16*& buffer) = 0;
        virtual void    getAudioFormat(int& samplerate, bool& stereo) = 0;

    protected:
        // sf::SoundStream inherited members
        virtual bool    onGetData(sf::SoundStream::Chunk& data) override;
        virtual void    onSeek(sf::Time timeOffset) override;

    private:
        Driver*         driver;
        void            initialize();
    };
}

#endif