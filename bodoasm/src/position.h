#ifndef BODOASM_POSITION_H_INCLUDED
#define BODOASM_POSITION_H_INCLUDED

namespace bodoasm
{
    struct Position
    {
        unsigned        fileId;
        unsigned        lineNo;
        unsigned        linePos;
    };
}

#endif
