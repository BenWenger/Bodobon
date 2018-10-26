
#include <stdexcept>
#include "stringpool.h"

namespace bodoasm
{
    std::unordered_map<std::string, unsigned>   StringPool::nameToId;
    std::vector<std::string>                    StringPool::idToName;
    
    std::string StringPool::get(unsigned id)
    {
        if(id >= idToName.size())
            throw std::runtime_error("Invalid ID passed to StringPool");
        return idToName[id];
    }

    unsigned StringPool::add(const std::string& str)
    {
        auto res = nameToId.insert( std::make_pair(str, idToName.size()) );
        if(res.second)
            idToName.push_back(str);
        return res.first->second;
    }
}
