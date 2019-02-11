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
        lua_createtable(lua, 0, static_cast<int>(obj.size()));
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
        lua_createtable(lua, static_cast<int>(ar.size()), 1);
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


    /////////////////////////////////////////////////////////////////

    json::value JsonFile::getJsonFromLua(luawrap::Lua& lua, int index)
    {
        luawrap::LuaStackSaver stk(lua);

        //put the value we're interested in at the top of the stack, just for convenience
        lua_pushvalue(lua, index);
        return getJsonFromLua_Value(lua);
    }
    
    json::value JsonFile::getJsonFromLua_Value(luawrap::Lua& lua)
    {
        switch(lua_type(lua, -1))
        {
        case LUA_TBOOLEAN:      return json::value( !!lua_toboolean(lua, -1) );
        case LUA_TSTRING:       return json::value( lua.toString(-1, false) );
        case LUA_TNUMBER:
            {
                if(lua_isinteger(lua, -1))      return json::value( static_cast<std::int64_t>( lua_tointeger(lua, -1) ) );
                else                            return json::value( static_cast<double>( lua_tonumber(lua, -1) ) );
            }break;
        case LUA_TTABLE:
            {
                // is this an object?  or an array?  Check the first key we find to see if it's a number.
                //    if a number, assume an array, otherwise assume an object
                bool isArray = false;
                lua_pushnil(lua);
                if(lua_next(lua, -2))
                {
                    isArray = (lua_type(lua, -2) == LUA_TNUMBER);
                    lua_pop(lua, 2);    // drop key and val
                }

                if(isArray)         return json::value( getJsonFromLua_Array(lua) );
                else                return json::value( getJsonFromLua_Object(lua) );
            }break;
        }

        return json::value();
    }
   
    json::object JsonFile::getJsonFromLua_Object(luawrap::Lua& lua)
    {
        // To fit with JSon, we have to restrict this to string keys
        json::object out;
        std::string key;

        lua_pushnil(lua);
        while(lua_next(lua,-2))
        {
            if(lua_type(lua, -2) == LUA_TSTRING)        // is it a string?
            {
                key = lua.toString(-2);
                out[key] = getJsonFromLua_Value(lua);
            }
            lua_pop(lua, 1);        // drop value to continue loop
        }

        return out;
    }
    
    json::array JsonFile::getJsonFromLua_Array(luawrap::Lua& lua)
    {
        // Strictly integer keys (>= 0)
        json::array out;
        int key;

        lua_pushnil(lua);
        while(lua_next(lua, -2))
        {
            if(lua_isinteger(lua, -2))          // integer key?
            {
                key = static_cast<int>( lua_tointeger(lua, -2) );
                if(key >= 0)
                {
                    if(key >= static_cast<int>(out.size()))     out.resize(key);
                    out[key] = getJsonFromLua_Value(lua);
                }
            }
            lua_pop(lua, 1);        // drop value to continue loop
        }
        
        return out;
    }
}