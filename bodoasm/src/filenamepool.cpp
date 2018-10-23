
#include <stdexcept>
#include "filenamepool.h"

namespace bodoasm
{
    std::unordered_map<std::string, unsigned>   FilenamePool::nameToId;
    std::vector<std::string>                    FilenamePool::idToName;
    
    std::string FilenamePool::get(unsigned fileid)
    {
        if(fileid >= idToName.size())
            throw std::runtime_error("Invalid ID passed to FilenamePool");
        return idToName[fileid];
    }

    unsigned FilenamePool::add(const std::string& filename)
    {
        auto res = nameToId.insert( std::make_pair(filename, idToName.size()) );
        if(res.second)
            idToName.push_back(filename);
        return res.first->second;
    }
}
