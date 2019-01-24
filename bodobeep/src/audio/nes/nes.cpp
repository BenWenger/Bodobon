
#include "nes.h"
#include "nativepulse.h"
#include "nativetriangle.h"

namespace bodobeep
{

    NesAudio::NesAudio()
    {
        channels[0] = std::make_unique<NativePulse>();
        channels[1] = std::make_unique<NativePulse>();
        channels[2] = std::make_unique<NativeTriangle>();

        synth.setFormat(sampleRate, cpuClockRate, frameCycs);
    }

    void NesAudio::addChannelsToLua(luawrap::Lua& lua)
    {
        luawrap::LuaStackSaver stk(lua);

        //  'bodo' table is currently on the stack
        lua_pushliteral(lua, "_channels");
        lua_newtable(lua);
        for(int i = 0; i < 3; ++i)          // TODO remove this '3' shit
        {
            lua.pushLightUserData(channels[i].get());
            lua_seti(lua, -2, i);
        }
        lua_settable(lua, -3);
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
