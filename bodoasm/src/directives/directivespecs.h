
#ifndef BODOASM_DIRECTIVESPECS_H_INCLUDED
#define BODOASM_DIRECTIVESPECS_H_INCLUDED

#include <string>
#include <unordered_map>
#include <vector>
#include "types/basetypes.h"

namespace bodoasm
{
    struct DirectiveParam
    {
        enum Type
        {
            Integer = 0,
            String,
            IntOrString,

            OptInteger,
            OptString,
            OptIntOrString,

            End,
            LoopBack                // must be last
        };

        int_t           valInt;
        std::string     valStr;
        Type            type;

        static Type loopBack(int count)
        {
            return static_cast<Type>( static_cast<int>(Type::LoopBack) + count );
        }

        static bool isTypeOptional(Type t)
        {
            switch(t)
            {
            case Type::OptInteger:
            case Type::OptString:
            case Type::OptIntOrString:
            case Type::End:
                return true;
            }
            return false;
        }

        static Type realType(Type t)
        {
            switch(t)
            {
            case Type::OptInteger:      return Type::Integer;
            case Type::OptString:       return Type::String;
            case Type::OptIntOrString:  return Type::IntOrString;
            }
            return t;
        }
    };

    typedef std::unordered_map<std::string, std::vector<DirectiveParam::Type>>      directiveSpec_t;
    typedef std::vector<DirectiveParam>     directiveParams_t;

    extern const directiveSpec_t    directiveSpecs;
}

#endif
