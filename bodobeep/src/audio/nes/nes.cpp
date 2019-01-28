
#include "nes.h"
#include "nativepulse.h"
#include "nativetriangle.h"

namespace bodobeep
{
    NesAudio::NesAudio()
    {
        synth.setFormat(sampleRate, cpuClockRate, frameCycs);
    }

    NesAudio::~NesAudio()
    {
    }

    void NesAudio::createChannels(luawrap::Lua& lua, const DriverSpec& spec)
    {
        std::set<std::string>       found;

        luawrap::LuaStackSaver stk(lua);
        for(auto& i : spec.channels)
        {
            if(!found.insert(i).second)
                throw std::runtime_error("Lua error:  bodo_driver has multiple channels named \"" + i + "\"");

            if(i == "pulse1" || i == "pulse2")
            {
                lua.pushString(i);
                channels.push_back(lua.pushNewUserData<NativePulse>(i));
                lua_settable(lua, -3);
            }
            else if(i == "triangle")
            {
                lua.pushString(i);
                channels.push_back(lua.pushNewUserData<NativeTriangle>(i));
                lua_settable(lua, -3);
            }
            // TODO noise / dmc / expansions
            else
                throw std::runtime_error("Lua error:  bodo_driver channel name \"" + i + "\" is unrecognized");
        }
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
