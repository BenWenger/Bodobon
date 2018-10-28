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
            std::size_t         tokenListSize;
        };

        virtual     ~SubParser() {};

        virtual void    warning(const Position* p, const std::string& msg) override;
        virtual void    error(const Position* p, const std::string& msg) override;

    protected:
                    SubParser(const Package& pkg, int maxRecursiveDepth);

        Token       next();
        void        back();
        void        skip(std::size_t count);

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


        std::size_t         getCurrentLexPos() const    { return pos;           }
        std::size_t         getCurrentLexSize() const   { return tokenListSize; }
        Package             buildSubPackage(std::size_t start, std::size_t stop);
        Package             buildForkSubPackage()       { return buildSubPackage(pos, tokenListSize);   }
        void                stopReportingErrors()       { errReport = nullptr;          }

    private:
        friend class RecursionMarker;
        ErrorReporter*      errReport;
        const Token*        tokenList;
        std::size_t         tokenListSize;
        std::size_t         pos;
        int                 maxDepth;
        int                 curDepth;
    };
}

#endif
