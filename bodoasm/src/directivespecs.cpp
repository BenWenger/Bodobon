
#include "directivespecs.h"

namespace bodoasm
{
    namespace
    {
        directiveSpec_t buildDirectiveSpecs()
        {
            typedef DirectiveParam          p;
            typedef DirectiveParam::Type    t;
            directiveSpec_t out;
            
            out["org"]    = { t::Integer,     t::Integer,        t::OptInteger,         t::OptInteger   };
            out["include"]= { t::String                                                                 };
            out["rebase"] = { t::Integer                                                                };
            out["byte"]   = { t::IntOrString, t::OptIntOrString, p::loopBack(1)                         };
            out["endbase"]= {};

            return out;
        }
    }

    
    // Yeah, yeah, it's a global
    const directiveSpec_t  directiveSpecs = buildDirectiveSpecs();

}