#include "jsonfile.h"
#include <stdexcept>

namespace bodobeep
{
    namespace
    {
        const char* const HostHeaderId = "Bodobeep host file";
        const char* const SongHeaderId = "Bodobeep song file";

        std::string getFileType(const json::object& obj)
        {
            auto i = obj.find("__header");
            if(i == obj.end())                  throw std::runtime_error("__header field missing");
            if(!i->second.is<json::object>())   throw std::runtime_error("__header field is not an object");

            auto hdr = i->second.get<json::object>();
            i = hdr.find("_fileType");
            if(i == hdr.end())                  throw std::runtime_error("__header field is missing _filetype field");
            if(!i->second.is<std::string>())    throw std::runtime_error("_fileType field is not a string");

            return i->second.get<std::string>();
        }
    }

    void JsonFile::pushJsonToLua(luawrap::Lua& lua, const json::object& obj)
    {
        lua_createtable(lua, 0, obj.size());
        for(auto& i : obj)
        {
            auto& name = i.first;
            if(!name.empty() && name[0] != '_')
            {
                lua.pushString(name);
                pushJsonToLua(lua, i.second);
                lua_settable(lua, -3);
            }
        }
    }

    void JsonFile::pushJsonToLua(luawrap::Lua& lua, const json::array& ar)
    {
        lua_createtable(lua, ar.size(), 1);
        for(std::size_t i = 0; i < ar.size(); ++i)
        {
            pushJsonToLua(lua, ar[i]);
            lua_seti(lua, -2, static_cast<lua_Integer>(i));
        }
    }

    void JsonFile::pushJsonToLua(luawrap::Lua& lua, const json::value& v)
    {
        if(v.is<double>())              lua_pushnumber(lua, v.get<double>());
        else if(v.is<json::object>())   pushJsonToLua(lua, v.get<json::object>());
        else if(v.is<json::array>())    pushJsonToLua(lua, v.get<json::array>());
        else if(v.is<std::string>())    lua.pushString( v.get<std::string>() );
        else if(v.is<std::int64_t>())   lua_pushinteger( lua, v.get<std::int64_t>() );
        else if(v.is<bool>())           lua_pushboolean( lua, v.get<bool>() ? 1 : 0 );
        else                            lua_pushnil( lua );
    }

    void JsonFile::load(std::istream& s)
    {
        json::value outVal;
        auto err = json::parse(outVal, s);
        if(!err.empty())                        throw std::runtime_error("Json parse error:  " + err);
        if(!outVal.is<json::object>())          throw std::runtime_error("Json file is not an object");

        auto& outObj = outVal.get<json::object>();
        auto typeName = getFileType(outObj);
        FileType ft = FileType::None;

        if(typeName == SongHeaderId)            ft = FileType::Song;
        else if(typeName == HostHeaderId)       ft = FileType::Host;
        else                                    throw std::runtime_error("Unrecognized file type");

        // TODO check version number

        fileType = ft;
        obj = std::move(outObj);
    }
}