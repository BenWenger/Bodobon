
#include <iostream>
#include <string>
#include "error.h"
#include "stringpool.h"

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
        output("Error", pos, msg);
        incError();
        throw Error();
    }

    void ErrorReporter::softError(const Position* pos, const std::string& msg)
    {
        output("Error", pos, msg);
        incError();
    }

    inline void ErrorReporter::incError()
    {
        ++errCount;
        if(errCount >= 100)
            fatal(nullptr, "Error count exceeded 100, halting assembly");
    }

    void ErrorReporter::fatal(const Position* pos, const std::string& msg)
    {
        ++errCount;
        output("Fatal Error", pos, msg);
        throw FatalError();
    }
    
    void ErrorReporter::output(const char* prefix, const Position* pos, const std::string& msg)
    {
        auto& out = std::cout;
        out << prefix << ": ";
        if(pos)
            out << formatPosition(*pos) << " ";
        out << msg << '\n';
    }

    std::string ErrorReporter::formatPosition(const Position& pos)
    {
        return "(" + StringPool::toStr(pos.fileId) + ": " + std::to_string(pos.lineNo) + ", " + std::to_string(pos.linePos+1) + ")";
    }

}