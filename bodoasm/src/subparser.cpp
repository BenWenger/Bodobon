
#include "subparser.h"
#include <algorithm>

namespace bodoasm
{
    SubParser::SubParser(const Package& pkg, int maxReDpth)
        : errReport(pkg.errReport)
        , tokenList(pkg.tokenList)
        , tokenListMax(pkg.tokenListSize-1)
        , pos(0)
        , maxDepth(maxReDpth)
        , curDepth(0)
    {}

    void SubParser::warning(const Position* p, const std::string& msg)
    {
        if(errReport)
            errReport->warning(p, msg);
    }

    void SubParser::error(const Position* p, const std::string& msg)
    {
        if(errReport)
            errReport->error(p, msg);
        else
            throw Error();
    }

    Token SubParser::next()
    {
        return tokenList[ std::min(tokenListMax, pos++) ];
    }

    void SubParser::back()
    {
        if(pos > 0)
            --pos;
    }
}