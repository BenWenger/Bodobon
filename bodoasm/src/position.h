#ifndef BODOASM_POSITION_H_INCLUDED
#define BODOASM_POSITION_H_INCLUDED

#include <memory>

namespace bodoasm
{
    struct Position
    {
        typedef std::shared_ptr<Position>   Ptr;
        unsigned                fileId = 0;
        unsigned                lineNo = 0;
        unsigned                linePos = 0;
        Ptr                     callStack = nullptr;
    };
}

#endif
