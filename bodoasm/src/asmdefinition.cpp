
#include <luawrap.h>
#include <algorithm>
#include "asmdefinition.h"
#include "stringpool.h"
#include "error.h"

using namespace luawrap;

namespace bodoasm
{
    namespace
    {
        std::string& makeLower(std::string& s)
        {
            for(auto& i : s)
            {
                if(i >= 'A' && i <= 'Z')
                    i += 'a'-'A';
            }
            return s;
        }

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

        lua->loadFile(path, debugname);
        lua->callFunction(0,0);

        // TODO add bodo callbacks here

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
                switch(lua_geti( *lua, -1, static_cast<lua_Integer>( pat.elements.size() + 1 ) ))
                {
                case LUA_TSTRING:
                    pat.elements.emplace_back(lua->toString(-1,false));
                    break;
                case LUA_TNUMBER:
                    switch(auto i = lua_tointeger(*lua, -1))
                    {
                    case 0:     pat.elements.emplace_back(Pattern::El::Type::String);       break;
                    case 1:     pat.elements.emplace_back(Pattern::El::Type::Integer);      break;
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

            // at this point the pattern is complete!
            //   put it in the mode table!
            auto modeid = StringPool::add(modename);
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
                auto id = StringPool::add(str);
                if(addrModePatterns.find(id) == addrModePatterns.end())
                    err.fatal(nullptr, "In lang Lua:  operand pattern name '" + str + "' appears in '" + name + "' table, but not in OperandPatterns table");
                modevec.push_back(id);
                lua_pop(*lua,1);
            }

            // Record it!
            if(!modevec.empty())
                mp.insert( std::make_pair( makeLower(key), std::move(removeDups(modevec)) ) );
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

        full = mnemonic;
        makeLower(full);

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

    /*
        vec_t           getAddrModeForMnemonic(std::string& mnemonic);
        const Pattern*  getPatternForAddrMode(unsigned addrmode);

        int             guessInstructionSize(const std::string& mnemonic, const AddrModeMatch* matches, int count);
        void            generateBinary(const std::string& mnemonic, const AddrModeMatch* match);
        */

}
