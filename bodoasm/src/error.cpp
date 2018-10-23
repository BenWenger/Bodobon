
#include <iostream>
#include "error.h"
#include "filenamepool.h"

namespace bodoasm
{
    
    ErrorReporter::ErrorReporter()
    {
        wrnCount = 0;
        errCount = 0;
    }

    void ErrorReporter::warning(const Position* pos, const std::string& msg)
    {
        ++wrnCount;
        output("Warning", pos, msg);
    }
    
    void ErrorReporter::error(const Position* pos, const std::string& msg)
    {
        ++errCount;
        output("Error", pos, msg);
        throw Error();
    }

    void ErrorReporter::fatal(const Position* pos, const std::string& msg)
    {
        ++errCount;
        output("Fatal Error", pos, msg);
        throw FatalError();
    }
    
    void ErrorReporter::output(const char* prefix, const Position* pos, const std::string& msg)
    {
        // This could theoretically throw a fatal error, so do this first
        std::string name;
        if(pos)         name = FilenamePool::get(pos->fileId);

        // at this point we should be good to go
        auto& out = std::cout;
        out << prefix << ": " << msg;
        if(pos)
        {
            out << "  (" << name << " :" << pos->lineNo << ", " << pos->linePos << ")";
        }
        out << '\n';
    }

}