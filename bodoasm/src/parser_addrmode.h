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
                            Parser_AddrMode(const Package& pkg, const Scope& scope, const Pattern& pattern);
        AddrModeExprs       parse();

    private:
                            Parser_AddrMode(const Parser_AddrMode& rhs) = default;
        Parser_AddrMode*    root;           // the "top level" parser to track recursion depth (can't use 'this')

        void                doParse();
        void                doMatch();
        void                doExpr();

        const Scope&        curScope;
        const Pattern&      pattern;
        std::size_t         patPos;             // position in the pattern
        
        AddrModeExprs       successfulOutput;
        bool                successful;         // output was successful

        AddrModeExprs       output;             // output so far

        std::size_t         doExpressionParse(std::size_t start, std::size_t stop, PatEl::Type type);
        void                tryForkToPatternMatch(std::size_t exprStart, std::size_t exprStop, PatEl::Type type);

        void                addSuccess();

        void                done();

    };
}

#endif
