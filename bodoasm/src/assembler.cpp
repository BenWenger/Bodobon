
#include <iomanip>
#include "assembler.h"
#include "parser.h"

using luawrap::Lua;

namespace bodoasm
{
    namespace
    {
        const char* const getPatternElTypeName(PatEl::Type t)
        {
            switch(t)
            {
            case PatEl::Type::Integer:       return "Integer";
            case PatEl::Type::String:        return "String";
            }
            return "<Internal error:  Unknown type>";
        }

        const char* const getExprTypeName(const Expression& e)
        {
            if(e.isInteger())               return "Integer";
            if(e.isString())                return "String";
            return "<Unresolved>";
        }
    }

    bool Assembler::dirTableBuilt = false;
    Assembler::dirTable_t Assembler::dirTable;

    void Assembler::buildDirTable()
    {
        if(!dirTableBuilt)
        {
            dirTableBuilt = true;
            dirTable["org"]         = &Assembler::directive_Org;
            dirTable["include"]     = &Assembler::directive_Include;
            dirTable["rebase"]      = &Assembler::directive_Rebase;
            dirTable["endbase"]     = &Assembler::directive_Endbase;
            dirTable["byte"]        = &Assembler::directive_Byte;
        }
    }

    Assembler::Assembler(const std::string& pathToLua, const std::string& asmmode)
        : lexer(err)
        , symbols(err)
        , asmDef(err)
    {
        PCEstablished = false;
        curPC = 0;
        unbasedPC = 0;
        clearCurOrgBlock();

        buildDirTable();
        asmDef.addFuncSupplier( std::bind(&Assembler::addLuaFuncs, this, std::placeholders::_1) );
        asmDef.load(pathToLua + asmmode + ".lua", (asmmode + ".lua").c_str() );
    }
    
    void Assembler::doFile(const std::string& path)
    {
        lexer.startFile(path);
        Parser::parse(this, &lexer, &asmDef, &symbols, err);
        int foo = 5;
    }

    void Assembler::addLuaFuncs(Lua& lua)
    {
        lua_pushliteral(lua, "getPC");
        lua.pushFunction(this, &Assembler::lua_getPC);
        lua_settable(lua, -3);
    }
    
    int Assembler::lua_getPC(luawrap::Lua& lua)
    {
        lua_pushinteger(lua, static_cast<lua_Integer>(curPC));
        return 1;
    }

    void Assembler::clearCurOrgBlock()
    {
        curOrgBlock.orgAddr = 0;
        curOrgBlock.fileOffset = 0;
        curOrgBlock.sizeCap = 0;
        curOrgBlock.fillVal = 0;
        curOrgBlock.dat.clear();
        curOrgBlock.hasSize = false;
        curOrgBlock.hasFill = false;
    }
    
    void Assembler::defineLabel(const Position& pos, const std::string& name)
    {
        if(!PCEstablished)      err.error(&pos, "Cannot create a label until PC has been established.  Please #org first");
        symbols.addSymbol(name, pos, Expression::buildInt(pos, curPC) );
    }

    void Assembler::defineSymbol(const Position& pos, const std::string& name, Expression::Ptr&& expr)
    {
        expr->eval(err, symbols, false);
        symbols.addSymbol(name, pos, std::move(expr));
    }

    void Assembler::doInstruction(const Position& pos, const std::string& mnemonic, AddrModeMatchMap&& matches)
    {
        if(!PCEstablished)      err.error(&pos, "Cannot output instructions until PC has been established.  Please #org first");

        if(resolveAndTypeMatch(matches, false))
        {
            int adj = asmDef.generateBinary(pos, mnemonic, matches, curOrgBlock.dat);
            curPC += adj;
            unbasedPC += adj;
        }
        else
        {
            int adj = asmDef.guessInstructionSize(pos, mnemonic, matches);

            // TODO -- add 'matches' to some kind of list of incomplete patches.

            for(int i = 0; i < adj; ++i)
                curOrgBlock.dat.push_back(0);
            curPC += adj;
            unbasedPC += adj;
        }
    }

    bool Assembler::resolveAndTypeMatch(AddrModeMatchMap& matches, bool force)
    {
        bool allResolved = true;
        auto mtch = matches.begin();
        while(mtch != matches.end())
        {
            bool typeOk = true;
            for(auto& i : mtch->second)
            {
                if(i.expr->eval(err, symbols, force))
                {
                    bool bad = i.expr->isInteger() && (i.type != PatEl::Type::Integer);
                    bad = bad || (i.expr->isString() && (i.type != PatEl::Type::String));

                    if(bad)
                    {
                        // We need to report an error IFF this is the only remaining match
                        if(matches.size() == 1)
                        {
                            err.error(&i.expr->getPos(), std::string("Type mismatch. Instruction was expecting ") + getPatternElTypeName(i.type)
                                      + " but expression resolves to " + getExprTypeName(*i.expr) );
                        }
                        else
                        {
                            // otherwise, break out and indicate our type was bad, so this match can be removed from the list
                            typeOk = false;
                            break;
                        }
                    }
                }
                else
                    allResolved = false;
            }

            // if the type was bad, remove this element from the matches.  Otherwise, just move to the next one
            if(typeOk)      ++mtch;
            else            mtch = matches.erase(mtch);
        }
        return allResolved;
    }

    void Assembler::doDirective(const Position& pos, const std::string& name, const directiveParams_t& params)
    {
        auto i = dirTable.find(name);
        if(i == dirTable.end())
            err.fatal(nullptr, "Internal error:  directive name '" + name + "' is not found in Assembler's directive table");
        (this->*(i->second))(pos, params);
    }

    void Assembler::directive_Byte(const Position& pos, const directiveParams_t& params)
    {
        if(!PCEstablished)      err.error(&pos, "Cannot output #byte values until PC has been established.  Please #org first");

        // TODO
    }

    void Assembler::directive_Org(const Position& pos, const directiveParams_t& params)
    {
        if(rebasing)                    err.warning(&pos, "#org reached while currently in a #rebase section.  #rebase section will be closed");
        if(params[1].valInt < 0)        err.error(&pos, "#org cannot accept an offset lower than zero");
        if(params.size() >= 4)
        {
            if(params[3].valInt < 0 || params[3].valInt > 0xFF)
                err.error(&pos, "#org fill byte must be within the range 0-255");
        }

        // Stash our current org block
        if(curOrgBlock.dat.size())
        {
            orgBlocks.emplace_back( std::move( curOrgBlock ) );
            clearCurOrgBlock();
        }

        // Start a new one
        curOrgBlock.orgAddr = params[0].valInt;
        curOrgBlock.fileOffset = params[1].valInt;
        if(params.size() >= 3)
        {
            curOrgBlock.hasSize = true;
            curOrgBlock.sizeCap = params[2].valInt;
            if(curOrgBlock.sizeCap < 0)
                curOrgBlock.sizeCap = curOrgBlock.fileOffset - curOrgBlock.sizeCap;
        }
        if(params.size() >= 4)
        {
            curOrgBlock.hasFill = true;
            curOrgBlock.fillVal = static_cast<int>(params[3].valInt);
        }
        
        PCEstablished = true;
        curPC = curOrgBlock.orgAddr;
        unbasedPC = curPC;
        rebasing = false;
    }

    void Assembler::directive_Include(const Position& pos, const directiveParams_t& params)
    {
        // TODO
        int foo = 4;
    }
    
    void Assembler::directive_Endbase(const Position& pos, const directiveParams_t& params)
    {
        if(!rebasing)               err.error(&pos, "#endbase reached when not in a #rebase section");
        curPC = unbasedPC;
        rebasing = false;
    }

    void Assembler::directive_Rebase(const Position& pos, const directiveParams_t& params)
    {
        if(!PCEstablished)          err.error(&pos, "PC has not been established. Cannot do a #rebase without a preceeding #org");
        if(rebasing)                err.error(&pos, "#rebase sections cannot be nested.  Please close existing #rebase with #endbase");

        unbasedPC = curPC;
        curPC = params[0].valInt;
        rebasing = true;
    }
}
