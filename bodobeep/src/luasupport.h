
#ifndef BODOBEEP_LUA_SUPPORT_H_INLCUDED
#define BODOBEEP_LUA_SUPPORT_H_INLCUDED



#define BEGIN_LUA_MEMBER_SET()                  auto s = lua.toString(-1);
#define LUA_MEMBER_SET(clsname, funcname)       if(s == #funcname)  lua.pushFunction(this, &clsname::lua_##funcname);   else
#define LUA_MEMBER_STRING(name, str)            if(s == name)       lua.pushString(str);                                else
#define END_LUA_MEMBER_SET()                    lua_pushnil(lua);       return 1;


#endif
