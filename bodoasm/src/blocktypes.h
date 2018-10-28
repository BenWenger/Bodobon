#ifndef BODOASM_BLOCKTYPES_H_INCLUDED
#define BODOASM_BLOCKTYPES_H_INCLUDED

#include "basetypes.h"
#include <vector>
#include <map>
#include "expression.h"

// there might be a better way to organize this....

namespace bodoasm
{
    class Expression;

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

    struct AddrModeExpr
    {
        Expression::Ptr         expr;
        Pattern::El::Type       type;
    };

    typedef std::vector<AddrModeExpr>           AddrModeExprs;
    typedef std::map<unsigned, AddrModeExprs>   AddrModeMatchMap;
}

#endif
