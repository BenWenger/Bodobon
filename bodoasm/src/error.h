#ifndef BODOASM_ERROR_H_INCLUDED
#define BODOASM_ERROR_H_INCLUDED

#include <stdexcept>
#include <string>
#include "position.h"

namespace bodoasm
{
    class Error : public std::exception {};
    class FatalError : public std::exception {};

    class ErrorReporter
    {
    public:
                            ErrorReporter();
        void                warning(const Position* pos, const std::string& msg);
        void                error(const Position* pos, const std::string& msg);
        void                fatal(const Position* pos, const std::string& msg);


    private:
        unsigned            wrnCount;
        unsigned            errCount;
        void                output(const char* prefix, const Position* pos, const std::string& msg);
    };
}

#endif