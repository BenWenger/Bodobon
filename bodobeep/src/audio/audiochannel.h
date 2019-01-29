#ifndef BODOBEEP_AUDIO_AUDIOCHANNEL_H_INCLUDED
#define BODOBEEP_AUDIO_AUDIOCHANNEL_H_INCLUDED

#include "types.h"
#include <luawrap.h>

class Driver;

namespace bodobeep
{
    class AudioChannel : public luawrap::LuaObject
    {
    public:
                                AudioChannel(const std::string& _name) : name(_name) {}
        virtual                 ~AudioChannel() {}

                    
        const std::string       name;
    protected:
        virtual void            addLuaMetaMethods(luawrap::Lua& lua) override;
        virtual void            pushMember(luawrap::Lua& lua, const std::string& name) = 0;
        
        int                     lua_index(luawrap::Lua& lua);
        int                     lua_newIndex(luawrap::Lua& lua);
    };
}

#endif
