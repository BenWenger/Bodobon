
#include "nes.h"
#include "nativepulse.h"
#include "nativetriangle.h"

namespace bodobeep
{

    NesAudio::NesAudio(luawrap::Lua& lua)
    {
        // TODO use the lua to determine which channels to create

        channels.emplace_back( std::make_unique<NativePulse>("pulse1") );
        channels.emplace_back( std::make_unique<NativePulse>("pulse2") );
        channels.emplace_back( std::make_unique<NativeTriangle>("triangle") );

        synth.setFormat(sampleRate, cpuClockRate, frameCycs);
    }

    std::set<std::string> NesAudio::addChannelsToLua(luawrap::Lua& lua)
    {
        std::set<std::string>        out;

        // The table we're adding to is already on the stack!
        for(auto& i : channels)
        {
            out.insert(i->name);
            lua.pushString(i->name);
            lua.pushLightUserData(i.get());
            lua_settable(lua, -3);
        }

        return out;
    }

    NesAudio::~NesAudio()
    {
    }

    
    void NesAudio::startAudio()
    {
        for(auto& i : channels)
            i->reset();
        synth.clear();
    }

    int NesAudio::getAudio(const s16*& buffer)
    {
        for(auto& i : channels)
            i->runForCycs(synth, frameCycs);
        return synth.generateSamples(buffer, frameCycs);
    }

    void NesAudio::getAudioFormat(int& samplerate, bool& stereo)
    {
        samplerate = this->sampleRate;
        stereo = false;
    }

}
