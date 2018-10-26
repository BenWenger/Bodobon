#ifndef BODOASM_BLOCKTYPES_H_INCLUDED
#define BODOASM_BLOCKTYPES_H_INCLUDED

#include "basetypes.h"
#include <vector>
#include <map>

// there might be a better way to organize this....

namespace bodoasm
{
    class Expression;

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


    struct Pattern
    {
        struct El
        {
            enum class Type
            {
                Match,          // this element is a pattern that needs to be matched
                String,         // this element is an expression that must resolve to a string
                Integer         // this element is an expression that must resolve to an integer
            };
            Type            type;
            std::string     match;  // that pattern to match if type==Match


            El(Type t) : type(t) {}
            El(const std::string& m) : type(Type::Match), match(m) {}
        };
        std::vector<El>     elements;
    };

    typedef std::vector<Expression*>            AddrModeExprs;
    typedef std::map<unsigned, AddrModeExprs>   AddrModeMatchMap;
}

#endif
