#ifndef BODOBEEP_JSONFILE_H_INCLUDED
#define BODOBEEP_JSONFILE_H_INCLUDED

#include <iostream>
#include "json.h"
#include <luawrap.h>

namespace bodobeep
{
    class JsonFile
    {
    public:
        enum class FileType
        {
            None,
            Host,
            Song
        };
        
        json::object                    obj;
        FileType                        fileType = FileType::None;

        void load(std::istream& s);
        void pushPublicToLua(luawrap::Lua& lua);

    private:
    };
}

#endif
