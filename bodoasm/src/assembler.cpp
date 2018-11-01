
#include <algorithm>
#include <iomanip>
#include "assembler.h"
#include "parser.h"
#include "stringpool.h"

#include <ctime>        // TODO remove these
#include <cstdlib>

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

    Assembler::Assembler(const std::string& pathToLua, const std::string& asmmode)
        : lexer(err)
        , symbols(err)
        , asmDef(err)
    {
        clearCurOrg();

        buildDirTable();
        asmDef.addFuncSupplier( std::bind(&Assembler::addLuaFuncs, this, std::placeholders::_1) );
        asmDef.load(pathToLua + asmmode + ".lua", (asmmode + ".lua").c_str() );
    }
    
    void Assembler::assembleFile(const std::string& path)
    {
        clearCurOrg();

        lexer.startFile(path);
        Parser::parse(this, &lexer, &asmDef, &symbols, err);
        if(err.getErrCount() > 0)       return;

        orgBlocks.emplace_back( std::move(curOrgBlock) );
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

    void Assembler::clearCurOrg()
    {
        PCEstablished = false;
        curPC = 0;
        unbasedPC = 0;
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

            Future fut;
            fut.mnemonic =          StringPool::toInt(mnemonic);
            fut.pos =               pos;
            fut.binaryPos =         static_cast<int>(curOrgBlock.dat.size());
            fut.matches =           std::move(matches);
            fut.promisedSize =      adj;
            fut.pcAtTime =          curPC;
            curOrgBlock.futures.emplace_back(std::move(fut));

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

    void Assembler::resolveFutures()
    {
        for(auto& blk : orgBlocks)
        {
            for(auto future : blk.futures)
            {
                curPC = future.pcAtTime;
                resolveAndTypeMatch(future.matches, true);
                asmDef.generateBinary(future.pos, StringPool::toStr(future.mnemonic), future.matches, blk.dat, future.binaryPos, future.promisedSize);
            }
        }
    }

    void Assembler::checkOrgConflicts()
    {
        // Check for size constraints, also pad out where necessary
        for(auto& i : orgBlocks)
        {
            if(i.hasSize && i.dat.size() > i.sizeCap)
                err.softError(&i.definePos, "Org block has exceeded it's allowed size");
            if(i.hasFill && i.dat.size() < i.sizeCap)
                i.dat.resize(static_cast<std::size_t>(i.sizeCap), static_cast<u8>(i.fillVal));
        }

        // To make this easier, sort out org blocks
        struct {
            bool operator () (const OrgBlock& a, const OrgBlock& b)
            {
                return a.fileOffset < b.fileOffset;
            }
        } sorter;
        std::sort(orgBlocks.begin(), orgBlocks.end(), sorter);

        int_t hi = 0;
        for(std::size_t i = 1; i < orgBlocks.size(); ++i)
        {
            const auto& prev = orgBlocks[i-1];
            const auto& now = orgBlocks[i];
            hi = std::max(hi, prev.fileOffset + prev.dat.size());
            if(hi > now.fileOffset)
                err.softError(&prev.definePos, "Org block overlaps another org block defined here: " + ErrorReporter::formatPosition(now.definePos));
        }

    }

    void Assembler::finalize()
    {
        try
        {
            resolveFutures();
            checkOrgConflicts();
        }
        catch(...)
        {
        }
    }

    void Assembler::output(dshfs::File::Ptr& file)
    {
        for(auto& blk : orgBlocks)
        {
            file->seek( blk.fileOffset );
            file->write( blk.dat.data(), blk.dat.size() );
        }
    }
    
    bool Assembler::okToProceed()
    {
        // TODO option for treating warnings as errors
        return err.getErrCount() == 0;
    }
}