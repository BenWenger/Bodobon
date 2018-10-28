
#ifndef BODOASM_DIRECTIVESPECS_H_INCLUDED
#define BODOASM_DIRECTIVESPECS_H_INCLUDED

#include <string>
#include <unordered_map>
#include <vector>
#include "basetypes.h"

namespace bodoasm
{
    struct DirectiveParam
    {
        enum class Type
        {
            Integer,
            String,
            OptInteger,
            OptString
        };

        int_t           valInt;
        std::string     valStr;
        Type            type;
    };

    typedef std::unordered_map<std::string, std::vector<DirectiveParam::Type>>      directiveSpec_t;
    typedef std::vector<DirectiveParam>     directiveParams_t;

    extern const directiveSpec_t    directiveSpecs;
}

#endif
