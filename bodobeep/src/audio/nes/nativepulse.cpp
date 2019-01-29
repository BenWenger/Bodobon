#include "nativepulse.h"
#include "audio/blsynth.h"
#include <map>
#include <string>
#include <algorithm>


namespace bodobeep
{
    namespace
    {
        const int dutyPhaseLut[4][0x10] = {
            {0,1,0,0,0,0,0,0},
            {0,1,1,0,0,0,0,0},
            {0,1,1,1,1,0,0,0},
            {1,0,0,1,1,1,1,1}
        };
    }

    NativePulse::NativePulse(const std::string& name)
        : NesChannel(name)
    {
        for(int i = 0; i < 0x10; ++i)
            outputLevels[i] = i * totalBase * pulseBase;
    }
    
    void NativePulse::pushMember(luawrap::Lua& lua, const std::string& name)
    {
        if     (name == "setVolume")        lua.pushFunction(this, &NativePulse::lua_setVolume);
        else if(name == "setPitch")         lua.pushFunction(this, &NativePulse::lua_setPitch);
        else if(name == "setDuty")          lua.pushFunction(this, &NativePulse::lua_setDuty);
    }

    void NativePulse::runForCycs(BlSynth& synth, timestamp_t cycs)
    {
        timestamp_t now = 0;
        while(cycs > 0)
        {
            auto ticks = std::min(cycs, freqCounter+1);
            
            cycs -= ticks;
            now += ticks;

            freqCounter -= ticks;
            while(freqCounter < 0)
            {
                freqCounter += (freqTimer + 1) * 2;
                dutyCounter = (dutyCounter + 1) & 0x07;
            }

            int out = 0;
            if(dutyPhaseLut[dutyCycle][dutyCounter])
                out = volume;

            if(out != curOut)
            {
                synth.addTransition( now, outputLevels[out] - outputLevels[curOut] );
                curOut = out;
            }
        }
    }
    
    int NativePulse::lua_setVolume(Lua& lua)
    {
        int isnum;
        auto v = lua_tointegerx(lua, 1, &isnum);
        if(isnum)
            volume = v & 0x0F;
        return 0;
    }

    int NativePulse::lua_setDuty(Lua& lua)
    {
        int isnum;
        auto v = lua_tointegerx(lua, 1, &isnum);
        if(isnum)
            dutyCycle = v & 0x03;
        return 0;
    }

    int NativePulse::lua_setPitch(Lua& lua)
    {
        // param 1 = the pitch
        int isnum;
        auto v = lua_tointegerx(lua, 1, &isnum);
        if(isnum)
            freqTimer = v & 0x07FF;

        // param 2 = boolean, true if phase is to reset
        if(lua_isboolean(lua, 2) && lua_toboolean(lua, 2))
        {
            freqCounter = (freqTimer + 1) * 2;
            dutyCounter = 0;
        }
        return 0;
    }

    void NativePulse::reset()
    {
        freqTimer = freqCounter = 0x07FF;
        volume = 0;
        dutyCounter = 0;
        dutyCycle = 0;
        curOut = 0;
    }

}
