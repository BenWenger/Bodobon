
#include "directivespecs.h"

namespace bodoasm
{
    namespace
    {
        directiveSpec_t buildDirectiveSpecs()
        {
            typedef DirectiveParam::Type    t;
            directiveSpec_t out;
            
            out["org"]    = { t::Integer, t::Integer, t::OptInteger, t::OptInteger };
            out["rebase"] = { t::Integer };

            return out;
        }
    }

    
    // Yeah, yeah, it's a global
    const directiveSpec_t  directiveSpecs = buildDirectiveSpecs();

}