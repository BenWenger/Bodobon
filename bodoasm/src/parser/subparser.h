#ifndef BODOASM_SUB_PARSER_H_INCLUDED
#define BODOASM_SUB_PARSER_H_INCLUDED

#include <string>
#include <vector>
#include "error/error.h"
#include "types/token.h"
#include "lexer/tokensource.h"

namespace bodoasm
{
    // this is derived from TokenSource, but that doesn't make much sense.
    //   Maybe instead of having the package here, I should have a separate class
    //   to hold that package and let subparsers use it.  More refactoring for later.
    class SubParser : public ErrorReporter, public BufferedTokenSource
    {
    public:
        struct Package
        {
            ErrorReporter*      errReport;
            const Token*        tokenList;
            std::size_t         tokenListSize;
        };
                            SubParser(const Package& pkg, int maxRecursiveDepth);
        std::size_t         getCurrentLexPos() const    { return pos - getUngottenCount();  }
        std::size_t         getCurrentLexSize() const   { return tokenListSize;             }

        virtual             ~SubParser() {};

        virtual void        warning(const Position* p, const std::string& msg) override;
        virtual void        error(const Position* p, const std::string& msg) override;

    protected:                    
        void        advance(std::size_t count);

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


        Package             buildSubPackage(std::size_t start, std::size_t stop);
        void                stopReportingErrors()       { errReport = nullptr;          }

    private:
        virtual Token       fetchToken() override;
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
