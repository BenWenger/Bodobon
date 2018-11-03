
#include <luawrap.h>
#include <algorithm>
#include "asmdefinition.h"
#include "types/stringpool.h"
#include "error/error.h"
#include "expression/expression.h"

using namespace luawrap;

namespace bodoasm
{
    namespace
    {
        AsmDefinition::vec_t& removeDups(AsmDefinition::vec_t& v)
        {
            std::sort( v.begin(), v.end() );
            v.erase( std::unique( v.begin(), v.end() ), v.end() );
            return v;
        }
    }

    AsmDefinition::AsmDefinition(ErrorReporter& er)
        : err(er)
    {
    }

    AsmDefinition::~AsmDefinition()
    {}

    void AsmDefinition::clear()
    {
        addrModePatterns.clear();
        mnemonicModes.clear();
        suffixModes.clear();
    }

    void AsmDefinition::load(const std::string& path, const char* debugname)
    {
        clear();

        lua = std::make_unique<Lua>();
        LuaStackSaver stk(*lua);

        // Create the 'bodoasm' global table and populate it
        //   with callbacks
        lua_newtable(*lua);
        for(auto& i : funcSuppliers)
            i(*lua);
        lua_setglobal(*lua, "bodoasm");

        lua->loadFile(path, debugname);
        lua->callFunction(0,0);

        auto x = lua_getglobal(*lua, "bodoasm_init");
        if(lua_type(*lua, -1) != LUA_TFUNCTION)
            err.fatal(nullptr, "In lang Lua:  global function 'bodoasm_init' must be defined");
        lua->callFunction(0, 1);
        if(lua_type(*lua, -1) != LUA_TTABLE)
            err.fatal(nullptr, "In lang Lua:  global function 'bodoasm_init' must return a table");

        loadAddrModes();        // this must be done first, so that addr modes can be recognized in future loads
        loadMnemonics();
        loadSuffixes();
    }
    
    void AsmDefinition::loadAddrModes()
    {
        LuaStackSaver stk(*lua);

        lua_pushliteral(*lua, "AddrModes");
        if(lua_gettable(*lua, -2) != LUA_TTABLE)
            return;

        // Loop through the outer table
        //      key = addr mode string
        //      value = table (array) containing the pattern to match
        lua_pushnil(*lua);
        while(lua_next(*lua, -2) != 0)
        {
            auto modename = lua->toString(-2, true);
            if(lua_type(*lua, -1) != LUA_TTABLE)
                err.fatal(nullptr, "In lang Lua:  Values in 'AddrModes' table must be an array");

            // build the pattern!
            Pattern pat;
            bool loop = true;
            while(loop)
            {
                switch(lua_geti( *lua, -1, static_cast<lua_Integer>( pat.size() + 1 ) ))
                {
                case LUA_TSTRING:
                    pat.emplace_back(lua->toString(-1,false));
                    if(pat.back().match.empty())                // empty strings are meaningless
                        err.fatal(nullptr, "In lang Lua:  'AddrModes' patterns cannot have empty strings");
                    break;
                case LUA_TNUMBER:
                    switch(auto i = lua_tointeger(*lua, -1))
                    {
                    case 0:     pat.emplace_back(PatEl::Type::String);       break;
                    case 1:     pat.emplace_back(PatEl::Type::Integer);      break;
                    default:    err.fatal(nullptr, "In lang Lua:  'AddrModes' integer code " + std::to_string(i) + " is invalid");
                    }
                    break;
                case LUA_TNIL: case LUA_TNONE:
                    loop = false;
                    break;
                default:
                    err.fatal(nullptr, "In lang Lua:  'AddrModes' patterns must be either a string or an integer");
                }
                lua_pop(*lua,1);        // drop the geti() value
            }

            // Patterns cannot have back-to-back expressions
            for(size_t i = 1; i < pat.size(); ++i)
            {
                if( pat[i].type   != PatEl::Type::Match &&
                    pat[i-1].type != PatEl::Type::Match )
                    err.fatal(nullptr, "In lang Lua:  'AddrModes' patterns cannot have back-to-back expressions");
            }

            // at this point the pattern is complete!
            //   put it in the mode table!
            auto modeid = StringPool::toInt(modename);
            addrModePatterns[modeid] = std::move(pat);

            lua_pop(*lua, 1);       // pop the value before continuing the loop
        }
    }
    
    void AsmDefinition::loadModeMap(const std::string& name, std::unordered_map<std::string,vec_t>& mp)
    {
        LuaStackSaver stk(*lua);

        lua->pushString(name);
        if(lua_gettable(*lua, -2) != LUA_TTABLE)
            return;

        // loop over the table, capture the mnemonics and their associated modes
        //      key = mnemonic
        //      value = table full of strings to indicate the modes available to this mnemonic
        lua_pushnil(*lua);
        while(lua_next(*lua, -2) != 0)
        {
            auto key = lua->toString(-2,true);
            if(lua_type(*lua, -1) != LUA_TTABLE)
                err.fatal(nullptr, "In lang Lua:  Values in '" + name + "' table must be arrays");

            // traverse the inner table to get compatible modes
            vec_t modevec;
            lua_pushnil(*lua);
            while(lua_next(*lua, -2) != 0)
            {
                auto str = lua->toString(-1,false);
                auto id = StringPool::toInt(str);
                if(addrModePatterns.find(id) == addrModePatterns.end())
                    err.fatal(nullptr, "In lang Lua:  operand pattern name '" + str + "' appears in '" + name + "' table, but not in OperandPatterns table");
                modevec.push_back(id);
                lua_pop(*lua,1);
            }

            // Record it!
            if(!modevec.empty())
                mp.insert( std::make_pair( toLower(key), std::move(removeDups(modevec)) ) );
            lua_pop(*lua, 1);       // pop the value before continuing the loop
        }
    }
    
    ////////////////////////////////////////////
    ////////////////////////////////////////////
    ////////////////////////////////////////////

    auto AsmDefinition::getAddrModeForMnemonic(const Position& pos, std::string& mnemonic) -> vec_t
    {
        if(mnemonic.empty())
            err.fatal(&pos,"Internal Error:  Dunno how this happened, but LuaInterface::getAddrModeForMnemonic got passed an empty string!");

        // The given mnemonic may or may not have a suffix.
        std::string full;

        full = toLower(mnemonic);

        // the easy out is a straight mnemonic with no suffix
        auto mn = mnemonicModes.find(full);
        if(mn != mnemonicModes.end())
        {
            mnemonic = full;
            return mn->second;
        }

        // otherwise, start breaking up the full mnemonic into 'mn'/main and 'sb'/sub parts, the sub part being the
        //   suffix
        decltype(mn)    sb;
        
        int i;
        for(i = static_cast<int>(full.size())-1; i > 0; --i)
        {
            sb = suffixModes.find( full.substr(i) );
            if(sb == suffixModes.end())                 continue;
            mn = mnemonicModes.find( full.substr(0,i) );
            if(mn == mnemonicModes.end())               continue;

            // we have a match!
            break;
        }

        if(i <= 0)      // no match
            err.error(&pos, "'" + mnemonic + "' is not a recognized mnemonic");

        // mn now points to the mnemonic portion
        // sb now points to the suffix portion
        //      build 'out' to be a logical AND of both mn and sb vectors
        vec_t out;
        auto a = mn->second.begin();
        auto b = sb->second.begin();
        while(a != mn->second.end() && b != sb->second.end())
        {
            if(*a == *b)
            {
                out.push_back(*a);
                ++a;    ++b;
            }
            else if(*a < *b)    ++a;
            else                ++b;
        }

        if(out.empty())
            err.error(&pos, "Suffix '" + sb->first + "' cannot be used with mnemonic '" + mn->first + "'");

        mnemonic = mn->first;
        return out;
    }
    
    
    ////////////////////////////////////////////
    ////////////////////////////////////////////
    ////////////////////////////////////////////

    const Pattern* AsmDefinition::getPatternForAddrMode(unsigned addrmode)
    {
        auto i = addrModePatterns.find(addrmode);
        if(i == addrModePatterns.end())
            err.fatal(nullptr, "Internal Error:  bad addrmode ID passed to getPatternForAddrMode");

        return &i->second;
    }

    void AsmDefinition::pushPatterns(const AddrModeMatchMap& patterns)
    {
        lua_createtable(*lua, 0, static_cast<int>(patterns.size()));
        for(auto& i : patterns)
        {
            lua->pushString( StringPool::toStr(i.first) );
            lua_createtable(*lua, static_cast<int>(i.second.size()), 0);
            for(std::size_t idx = 0; idx < i.second.size(); ++idx)
            {
                auto expr = i.second[idx].expr.get();
                if(expr->isInteger())           lua_pushinteger(*lua, static_cast<lua_Integer>(expr->asInteger()));
                else if(expr->isString())       lua->pushString(expr->asString());
                else                            lua_pushnil(*lua);
                
                lua_seti( *lua, -2, static_cast<lua_Integer>(idx+1) );
            }
            lua_settable( *lua, -3 );
        }
    }

    int AsmDefinition::guessInstructionSize(const Position& pos, const std::string& mnemonic, AddrModeMatchMap& patterns)
    {
        LuaStackSaver stk(*lua);
        if(lua_getglobal(*lua, "bodoasm_guessSize") != LUA_TFUNCTION)
            err.error(&pos, "In lang Lua:  global function 'bodo_guessSize' must be defined");
        lua->pushString(mnemonic);
        pushPatterns(patterns);

        try
        {
            lua->callFunction(2, 2);
        } catch(std::exception& e) {
            err.error(&pos, e.what());
        }

        if(!lua_isinteger(*lua, -2))
            err.error(&pos, "In lang Lua:  'bodo_guessSize' first return value must be an integer");
        int result = static_cast<int>(lua_tointeger(*lua, -2));

        switch(lua_type(*lua, -1))
        {
        case LUA_TNIL:  case LUA_TNONE:         // optional, just ignore it
            break;
        case LUA_TTABLE:
            {
                // Here, we need to get all the modes they supplied and filter them
                AddrModeMatchMap newpats;

                lua_pushnil(*lua);
                while(lua_next(*lua, -2) != 0)
                {
                    if(!lua_isstring(*lua,-1))
                        err.error(&pos, "In lang Lua:  'bodo_guessSize' second return value, if exists, must be an array of strings");
                    auto id = StringPool::toInt( lua->toString(-1,false) );
                    auto iter = patterns.find(id);
                    if(iter != patterns.end())
                    {
                        newpats.insert({id, std::move(iter->second)});
                        patterns.erase(iter);
                    }
                    lua_pop(*lua, 1);
                }
                patterns = std::move(newpats);
            }break;
        default:
            err.error(&pos, "In lang Lua:  'bodo_guessSize' second return value, if exists, must be an array of strings");
        }

        return result;
    }

    int AsmDefinition::generateBinary(const Position& pos, const std::string& mnemonic, const AddrModeMatchMap& patterns, std::vector<u8>& bin, int insertoffset, int requiredsize)
    {
        auto toppos = lua_gettop(*lua)+1;       // I really hate Lua's 1 based indexing

        LuaStackSaver stk(*lua);
        if(lua_getglobal(*lua, "bodoasm_getBinary") != LUA_TFUNCTION)
            err.error(&pos, "In lang Lua:  global function 'bodoasm_getBinary' must be defined");
        lua->pushString(mnemonic);
        pushPatterns(patterns);
        
        int numvals;
        try
        {
            numvals = lua->callFunction(2, -1);
        } catch(std::exception& e) {
            err.error(&pos, e.what());
        }

        if(numvals <= 0)
            err.error(&pos, "In lang Lua:  'bodoasm_getBinary' must return a series of bytes to output");
        if((requiredsize > 0) && (numvals != requiredsize))
            err.error(&pos, "In lang Lua:  'bodoasm_getBinary' returned " + std::to_string(numvals) + " bytes, but assembler was previously promised " + std::to_string(requiredsize));

        constexpr const char* errmsg = "In lang Lua:  'bodoasm_getBinary' must return an array of unsigned integers within the range 0-255";

        for(int i = 0; i < numvals; ++i)
        {
            int v = -1;
            if(lua_isinteger(*lua, toppos+i))
                v = static_cast<int>(lua_tointeger(*lua, toppos+i));

            if(v < 0 || v > 255)
                err.error(&pos, "In lang Lua:  'bodoasm_getBinary' must return an array of unsigned integers within the range 0-255");

            if(requiredsize > 0)        bin[insertoffset+i] = static_cast<u8>(v);
            else                        bin.push_back(static_cast<u8>(v));
        }

        return numvals;
    }

}
