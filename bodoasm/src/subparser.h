#ifndef BODOASM_SUB_PARSER_H_INCLUDED
#define BODOASM_SUB_PARSER_H_INCLUDED

#include <string>
#include "error.h"
#include "token.h"

namespace bodoasm
{
    class SubParser : public ErrorReporter
    {
    public:
        struct Package
        {
            ErrorReporter*      errReport;
            const Token*        tokenList;
            std::size_t         tokenListSize;      // exclusive
        };

        virtual     ~SubParser() {};

        virtual void    warning(const Position* p, const std::string& msg) override;
        virtual void    error(const Position* p, const std::string& msg) override;

    protected:
                    SubParser(const Package& pkg, int maxRecursiveDepth);

        Token       next();
        void        back();

        class       RecursionMarker
        {
        public:
            RecursionMarker(SubParser* par, const Position* pos)
                : ptr(par)
            {
                if(++ptr->curDepth >= ptr->maxDepth)    ptr->error(pos, "Maximum recursion depth reached. Statement is too complex");
            }

            ~RecursionMarker()
            {
                --ptr->curDepth;
            }

        private:
            SubParser* ptr;
        };

    private:
        friend class RecursionMarker;
        ErrorReporter*      errReport;
        const Token*        tokenList;
        std::size_t         tokenListMax;           // inclusive
        std::size_t         pos;
        int                 maxDepth;
        int                 curDepth;
    };
}

#endif
