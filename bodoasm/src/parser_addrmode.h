#ifndef BODOASM_PARSER_ADDRMODE_H_INCLUDED
#define BODOASM_PARSER_ADDRMODE_H_INCLUDED

#include <string>
#include <vector>
#include "error.h"
#include "expression.h"
#include "subparser.h"
#include "blocktypes.h"

namespace bodoasm
{
    class Parser_AddrMode : public SubParser
    {
    public:
                            Parser_AddrMode(const Package& pkg, const std::string& scope, const Pattern* pattern);
        AddrModeExprs       parse();

    private:
        std::string         curScope;
        std::size_t         patPos = 0;     // position in the pattern
        const Pattern*      pattern;

        void                doMatch();
        void                doExpr();

        struct ExprRange
        {
            Pattern::El::Type   type;
            std::size_t         start;
            std::size_t         stop;
        };
        std::vector<ExprRange>  exprRanges;

        struct Fork
        {
            Package                 pkg;
            std::size_t             patPos;
            std::vector<ExprRange>  exprRanges;
        };
        std::vector<Fork>       forks;
    };
}

#endif
