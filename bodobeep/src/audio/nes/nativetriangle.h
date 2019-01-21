#ifndef BODOBEEP_AUDIO_NES_NATIVETRIANGLE_H_INCLUDED
#define BODOBEEP_AUDIO_NES_NATIVETRIANGLE_H_INCLUDED

#include "types.h"
#include "neschannel.h"
#include <luawrap.h>
#include <map>

namespace bodobeep
{
    class NativeTriangle : public luawrap::LuaObject, public NesChannel
    {
    public:
                        NativeTriangle();
        virtual void    runForCycs(BlSynth& synth, timestamp_t cycs) override;
        virtual void    reset() override;

    protected:
        typedef luawrap::Lua        Lua;
        
        virtual int     indexHandler(Lua& lua) override;

    private:
        int             lua_setVolume(Lua& lua);
        int             lua_setPitch(Lua& lua);
        
        bool            audible;
        int             freqCounter;
        int             freqTimer;
        int             triStep;

        int             curOut;
        float           outputLevels[0x10];
    };
}

#endif 