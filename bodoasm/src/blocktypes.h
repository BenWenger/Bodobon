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
    
    //  Pattern elements
    struct PatEl
    {
        enum class Type
        {
            Match,          // this element is a pattern that needs to be matched
            String,         // this element is an expression that must resolve to a string
            Integer         // this element is an expression that must resolve to an integer
        };
        Type            type;
        std::string     match;  // that pattern to match if type==Match


        PatEl(Type t) : type(t) {}
        PatEl(const std::string& m) : type(Type::Match), match(m) {}
    };

    typedef std::vector<PatEl>      Pattern;

    struct AddrModeExpr
    {
        Expression::Ptr expr;
        PatEl::Type     type;
    };

    typedef std::vector<AddrModeExpr>           AddrModeExprs;
    typedef std::map<unsigned, AddrModeExprs>   AddrModeMatchMap;

    struct Scope
    {
        std::string     topLabel;
        unsigned        nextUnnamed = 0;

        std::string     getNamelessName(int adj = 0) const
        {
            return topLabel + ".#nmls_" + std::to_string(nextUnnamed + adj);
        }
    };


    struct Token;
    struct Macro
    {
        Position                    definePos;
        std::vector<std::string>    paramNames;
        std::vector<Token>          tokens;
    };
}

#endif
