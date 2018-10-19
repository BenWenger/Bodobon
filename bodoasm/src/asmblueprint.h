
#ifndef BODOASM_ASMBLUEPRINT_H_INCLUDED
#define BODOASM_ASMBLUEPRINT_H_INCLUDED

#include <memory>
#include <luawrap.h>

namespace bodoasm
{
    class AsmBlueprint
    {
    public:
        void            loadBlueprint(const std::string& bpname);

    private:
        luawrap::Lua    lua;
    };
}

#endif