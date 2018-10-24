#ifndef BODOASM_BLOCKTYPES_H_INCLUDED
#define BODOASM_BLOCKTYPES_H_INCLUDED

#include "basetypes.h"
#include <vector>
#include <map>

// there might be a better way to organize this....

namespace bodoasm
{
    struct OrgBlock
    {
        int_t               orgAddr;
        int_t               fileOffset;
        std::vector<u8>     dat;
    };

    struct UnfinishedCmd
    {
        unsigned            orgBlockId;
        unsigned            size;
        //  TODO more here
    };
}

#endif
