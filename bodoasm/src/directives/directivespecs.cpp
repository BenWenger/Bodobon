
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
            out["rebase"]   = { t::Integer                                                                  };
            out["endbase"]  = {                                                                             };
            out["pushtable"]= { t::OptString                                                                };
            out["settable"] = { t::OptString                                                                };
            out["poptable"] = {                                                                             };
            out["if"]       = { t::Integer                                                                  };
            out["elif"]     = { t::Integer                                                                  };
            out["else"]     = {                                                                             };
            out["endif"]    = {                                                                             };
            out["pad"]      = { t::Integer,     t::OptInteger                                               };
            out["align"]    = { t::Integer,     t::OptInteger                                               };
            out["byte"]     = { t::IntOrString, t::OptIntOrString, p::loopBack(1)                           };
            out["word"]     = { t::Integer,     t::OptInteger,     p::loopBack(1)                           };
            out["long"]     = { t::Integer,     t::OptInteger,     p::loopBack(1)                           };
            out["dword"]    = { t::Integer,     t::OptInteger,     p::loopBack(1)                           };
            out["warning"]  = { t::String                                                                   };
            out["error"]    = { t::String                                                                   };

            return out;
        }
    }

    
    // Yeah, yeah, it's a global
    const directiveSpec_t  directiveSpecs = buildDirectiveSpecs();

}