#ifndef BODOBEEP_AUDIO_NES_NATIVETRIANGLE_H_INCLUDED
#define BODOBEEP_AUDIO_NES_NATIVETRIANGLE_H_INCLUDED

#include "types.h"
#include "neschannel.h"
#include <luawrap.h>
#include <map>

namespace bodobeep
{
    class NativeTriangle : public NesChannel
    {
    public:
                        NativeTriangle(const std::string& name);
        virtual void    runForCycs(BlSynth& synth, timestamp_t cycs) override;
        virtual void    reset() override;

    protected:
        typedef luawrap::Lua        Lua;
        
        virtual void    pushMember(luawrap::Lua& lua, const std::string& name) override;

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