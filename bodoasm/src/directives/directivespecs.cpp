
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
            
            out["org"]      = { t::Integer,     t::Integer,        t::OptInteger,         t::OptInteger     };
            out["include"]  = { t::String                                                                   };
            out["byte"]     = { t::IntOrString, t::OptIntOrString, p::loopBack(1)                           };
            out["rebase"]   = { t::Integer                                                                  };
            out["endbase"]  = {                                                                             };
            out["pushtable"]= { t::String                                                                   };
            out["poptable"] = {                                                                             };
            out["if"]       = { t::Integer                                                                  };
            out["elif"]     = { t::Integer                                                                  };
            out["else"]     = {                                                                             };
            out["endif"]    = {                                                                             };

            return out;
        }
    }

    
    // Yeah, yeah, it's a global
    const directiveSpec_t  directiveSpecs = buildDirectiveSpecs();

}