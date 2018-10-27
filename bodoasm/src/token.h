#ifndef BODOASM_TOKEN_H_INCLUDED
#define BODOASM_TOKEN_H_INCLUDED

#include <string>
#include "basetypes.h"
#include "position.h"

namespace bodoasm
{
    class ErrorReporter;
    struct Token
    {
        enum class Type 
        {
            Operator,
            Misc,               // symbols and numeric literals... any amalgomation of alphanumeric and underscores (before resolving)
            String,
            CmdEnd,             // end of command (end of line or \ symbol)
            FileEnd,            // end of a single file
            InputEnd,           // end of all input (file fully traversed)

            // What 'Misc' can become after resolving:
            Symbol,
            Integer
        };
        Position                    pos;

        std::string                 str;                    // the operator/symbol (but not for string literals)
        std::string                 strVal;                 // the string (for string literals only)
        int_t                       val = 0;                // the integer
        Type                        type = Type::InputEnd;
        bool                        ws_after = true;        // true if there is whitespace after this token

        bool isEnd() const
        {
            switch(type)
            {
            case Type::CmdEnd:
            case Type::FileEnd:
            case Type::InputEnd:
                return true;
            }
            return false;
        }

        bool isPossibleSymbol() const
        {
            if(type == Type::Symbol)            return true;
            if(type != Type::Misc)              return false;
            if(str.empty())                     throw std::runtime_error("Internal error:  Somehow got an empty string for a 'Misc' token");
            if(str[0] >= '0' && str[0] <= '9')  return false;
            return true;
        }

        Token resolve(ErrorReporter& err, int numbase=0) const;

    private:
        Token resolve_bin(ErrorReporter& err) const;
        Token resolve_dec(ErrorReporter& err) const;
        Token resolve_hex(ErrorReporter& err) const;
    };
}

#endif