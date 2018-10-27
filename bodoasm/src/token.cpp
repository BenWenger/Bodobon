
#include "token.h"
#include "error.h"

namespace bodoasm
{

    Token Token::resolve(ErrorReporter& err, int numbase) const
    {
        if(type != Type::Misc)      return *this;
        if(str.empty())             err.fatal(&pos, "Internal Error:  Somehow Token::resolve has an empty string!");

        if(numbase == 16)                   return resolve_hex(err);
        if(numbase == 10)                   return resolve_dec(err);
        if(numbase == 2)                    return resolve_bin(err);

        if(str[0] >= '0' && str[0] <= '9')  return resolve_dec(err);

        Token out(*this);
        out.type = Type::Symbol;
        return out;
    }

    Token Token::resolve_bin(ErrorReporter& err) const
    {
        Token out(*this);
        out.str.clear();
        out.type = Type::Integer;
        out.val = 0;

        for(auto& c : str)
        {
            out.val <<= 1;
            if(c == '1')        out.val += 1;
            else if(c != '0')   err.error(&pos, std::string("Unexpected character in binary literal:  '") + c + "'");

            if(out.val < 0)     err.error(&pos, "Numeric literal too large");
        }
        return out;
    }
    
    Token Token::resolve_dec(ErrorReporter& err) const
    {
        Token out(*this);
        out.str.clear();
        out.type = Type::Integer;
        out.val = 0;

        for(auto& c : str)
        {
            out.val *= 10;
            if(c >= '0' && c <= '9')    out.val += c-'0';
            else                        err.error(&pos, std::string("Unexpected character in numeric literal:  '") + c + "'");
            if(out.val < 0)             err.error(&pos, "Numeric literal too large");
        }
        return out;
    }
    
    Token Token::resolve_hex(ErrorReporter& err) const
    {
        Token out(*this);
        out.str.clear();
        out.type = Type::Integer;
        out.val = 0;

        for(auto& c : str)
        {
            out.val *= 16;
            if(c >= '0' && c <= '9')        out.val += c-'0';
            else if(c >= 'A' && c <= 'F')   out.val += c-'A'+10;
            else if(c >= 'a' && c <= 'f')   out.val += c-'a'+10;
            else                            err.error(&pos, std::string("Unexpected character in hex literal:  '") + c + "'");
            if(out.val < 0)                 err.error(&pos, "Numeric literal too large");
        }
        return out;
    }
}
