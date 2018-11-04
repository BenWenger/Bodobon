#ifndef BODOASM_TOKENSOURCE_H_INCLUDED
#define BODOASM_TOKENSOURCE_H_INCLUDED

#include <stdexcept>
#include "types/token.h"

namespace bodoasm
{
    class TokenSource
    {
    public:
        virtual             ~TokenSource() = default;
        virtual Token       next() = 0;
        virtual void        back() = 0;
        virtual void        back(int count)
        {
            for(int i = 0; i < count; ++i)      back();
        }
    };


    class BufferedTokenSource : public TokenSource
    {
    protected:
        virtual Token           fetchToken() = 0;

        void clearTokenBuffer()
        {
            pos = bufMask;
            countBack = 0;
            countAhead = 0;
        }

    public:
        virtual Token next() override
        {
            pos = (pos+1) & bufMask;
            if(countAhead > 0)
            {
                --countAhead;
                ++countBack;
            }
            else
            {
                if(countBack < bufSize)     ++countBack;
                buffer[pos] = fetchToken();
            }
            return buffer[pos];
        }

        virtual void back() override
        {
            if(countBack <= 0)
                throw std::runtime_error("Internal error:  BufferedTokenSource backtrack buffer exceeded.");

            ++countAhead;
            --countBack;
            pos = (pos+bufMask) & bufMask;
        }

    private:
        static const int        bufSize = 8;
        static const int        bufMask = 7;
        int                     pos = bufMask;
        int                     countBack = 0;
        int                     countAhead = 0;
        Token                   buffer[bufSize];
    };
}

#endif

