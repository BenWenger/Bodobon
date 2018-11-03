#ifndef BODOASM_ERROR_H_INCLUDED
#define BODOASM_ERROR_H_INCLUDED

#include <stdexcept>
#include <string>
#include "types/position.h"

namespace bodoasm
{
    class Error : public std::exception {};
    class FatalError : public std::exception {};

    class ErrorReporter
    {
    public:
                            ErrorReporter();
        virtual void        warning(const Position* pos, const std::string& msg);
        virtual void        error(const Position* pos, const std::string& msg);
        void                fatal(const Position* pos, const std::string& msg);
        static std::string  formatPosition(const Position& pos);

        unsigned            getErrCount() const         { return errCount; }
        unsigned            getWarningCount() const     { return wrnCount; }

        void                softError(const Position* pos, const std::string& msg);


    private:
        unsigned            wrnCount;
        unsigned            errCount;
        void                output(const char* prefix, const Position* pos, const std::string& msg);
        void                incError();
    };
}

#endif