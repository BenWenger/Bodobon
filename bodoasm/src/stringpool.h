#ifndef BODOASM_FILENAMEPOOL_H_INCLUDED
#define BODOASM_FILENAMEPOOL_H_INCLUDED

#include <string>
#include <vector>
#include <unordered_map>

namespace bodoasm
{
    class StringPool
    {
    public:
        static std::string      get(unsigned id);
        static unsigned         add(const std::string& str);

    private:
        ~StringPool() = delete;
        static std::unordered_map<std::string, unsigned>        nameToId;
        static std::vector<std::string>                         idToName;
    };
}

#endif
