#include "nativetriangle.h"
#include "audio/blsynth.h"
#include <map>
#include <string>
#include <algorithm>

namespace bodobeep
{
    NativeTriangle::NativeTriangle(const std::string& name)
        : NesChannel(name)
    {
        for(int i = 0; i < 0x10; ++i)
            outputLevels[i] = i * totalBase * triangleBase;
    }
    
    void NativeTriangle::pushMember(luawrap::Lua& lua, const std::string& name)
    {
        if     (name == "setVolume")        lua.pushFunction(this, &NativeTriangle::lua_setVolume);
        else if(name == "setPitch")         lua.pushFunction(this, &NativeTriangle::lua_setPitch);
    }

    void NativeTriangle::runForCycs(BlSynth& synth, timestamp_t cycs)
    {
        timestamp_t now = 0;
        while(cycs > 0)
        {
            auto ticks = std::min(cycs, freqCounter+1);
            if(!audible)
                ticks = cycs;
            
            cycs -= ticks;
            now += ticks;

            if(audible)
            {
                freqCounter -= ticks;
                while(freqCounter < 0)
                {
                    freqCounter += freqTimer + 1;
                    triStep = (triStep + 1) & 0x1F;
                }
            }

            int out = triStep;
            if(out & 0x10)
                out ^= 0x1F;

            if(out != curOut)
            {
                synth.addTransition( now, outputLevels[out] - outputLevels[curOut] );
                curOut = out;
            }
        }
    }
    
    int NativeTriangle::lua_setVolume(Lua& lua)
    {
        int isnum;
        auto v = lua_tointegerx(lua, 1, &isnum);
        if(isnum)
            audible = v != 0;
        return 0;
    }

    int NativeTriangle::lua_setPitch(Lua& lua)
    {
        int isnum;
        auto v = lua_tointegerx(lua, 1, &isnum);
        if(isnum)
            freqTimer = v & 0x07FF;
        return 0;
    }

    void NativeTriangle::reset()
    {
        audible = false;
        freqCounter = freqTimer = 0x07FF;
        triStep = 0;
        curOut = 0;
    }

}
