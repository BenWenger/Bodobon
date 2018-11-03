#ifndef BODOASM_BASETYPES_H_INCLUDED
#define BODOASM_BASETYPES_H_INCLUDED

#include <string>
#include <cstdint>

namespace bodoasm
{
    typedef std::uint8_t            u8;
    typedef std::int_fast64_t       int_t;

    // This doesn't really belong here, but I didn't want to make a separate header for it, so whatever
    inline char toLower(char i)
    {
        if(i >= 'A' && i <= 'Z')
            return i + 'a' - 'A';
        return i;
    }
    
    inline std::string toLower(const std::string& in)
    {
        std::string out;
        out.reserve(in.size());

        for(auto& i : in)
            out.push_back(toLower(i));
        return out;
    }
}

#endif
