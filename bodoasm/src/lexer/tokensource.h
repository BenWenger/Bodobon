#ifndef BODOASM_TOKENSOURCE_H_INCLUDED
#define BODOASM_TOKENSOURCE_H_INCLUDED

#include <vector>
#include "types/token.h"

namespace bodoasm
{
    class TokenSource
    {
    public:
        virtual             ~TokenSource() = default;
        virtual Token       next() = 0;
        virtual void        unget(const Token& t) = 0;
    };

    class BufferedTokenSource : public TokenSource
    {
    public:
        virtual             ~BufferedTokenSource() = default;
        virtual Token       next() override
        {
            if(ungotten.empty())
                return fetchToken();
            Token out = ungotten.back();
            ungotten.pop_back();
            return out;
        }
        virtual void        unget(const Token& t) override
        {
            ungotten.push_back(t);
        }

    protected:
        std::size_t             getUngottenCount() const { return ungotten.size();      }

    private:
        std::vector<Token>      ungotten;
        virtual Token           fetchToken() = 0;
    };

}

#endif

