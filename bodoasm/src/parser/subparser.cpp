
#include "subparser.h"
#include <algorithm>

namespace bodoasm
{
    SubParser::SubParser(const Package& pkg, int maxReDpth)
        : errReport(pkg.errReport)
        , tokenList(pkg.tokenList)
        , tokenListSize(pkg.tokenListSize)
        , pos(0)
        , maxDepth(maxReDpth)
        , curDepth(0)
    {
        if(tokenListSize == 0)
            throw std::runtime_error("Fatal Internal Error:  SubParser ctor given a zero-length package");
    }

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
        Token out;
        if(pos >= tokenListSize)
        {
            out.pos = tokenList[ tokenListSize-1 ].pos;
            out.type = Token::Type::CmdEnd;
        }
        else
            out = tokenList[pos];
        ++pos;
        return out;
    }
    
    void SubParser::advance(std::size_t count)
    {
        pos += count;
    }

    void SubParser::back()
    {
        if(pos > 0)
            --pos;
    }
    
    auto SubParser::buildSubPackage(std::size_t start, std::size_t stop) -> Package
    {
        Package p;
        p.errReport = errReport;
       
        start = std::min(start, tokenListSize-1);
        stop  = std::min(stop, tokenListSize);

        p.tokenList = tokenList + start;
        p.tokenListSize = stop - start;
        
        return p;
    }
}