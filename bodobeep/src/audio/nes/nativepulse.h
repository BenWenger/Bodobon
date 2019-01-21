#ifndef BODOBEEP_AUDIO_NES_NATIVEPULSE_H_INCLUDED
#define BODOBEEP_AUDIO_NES_NATIVEPULSE_H_INCLUDED

#include "types.h"
#include <luawrap.h>
#include <map>
#include "neschannel.h"

namespace bodobeep
{
    class NativePulse : public luawrap::LuaObject, public NesChannel
    {
    public:
                        NativePulse();
        virtual void    runForCycs(BlSynth& synth, timestamp_t cycs) override;
        virtual void    reset() override;

    protected:
        typedef luawrap::Lua        Lua;
        
        virtual int     indexHandler(Lua& lua) override;

    private:
        int             lua_setVolume(Lua& lua);
        int             lua_setDuty(Lua& lua);
        int             lua_setPitch(Lua& lua);


        int             freqCounter;
        int             freqTimer;
        int             volume;
        int             dutyCounter;
        int             dutyCycle;

        int             curOut;
        float           outputLevels[0x10];
    };
}

#endif 