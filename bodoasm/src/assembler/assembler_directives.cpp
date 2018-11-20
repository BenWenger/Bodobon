#include "assembler.h"
#include "types/stringpool.h"
#include <dshfs.h>

namespace bodoasm
{
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
            dirTable["pushtable"]   = &Assembler::directive_PushTable;
            dirTable["poptable"]    = &Assembler::directive_PopTable;
            dirTable["pad"]         = &Assembler::directive_Pad;
            dirTable["align"]       = &Assembler::directive_Align;
        }
    }
    
    namespace
    {
        // Make a path relative to the current file in the given position
        std::string makePath(const Position& pos, const std::string& filename)
        {
            dshfs::Filename fn;
            fn.setFullPath( StringPool::toStr(pos.fileId) );
            return fn.getPathPart() + filename;
        }
    }

    void Assembler::directive_Byte(const Position& pos, const directiveParams_t& params)
    {
        if(!PCEstablished)      err.error(&pos, "Cannot output #byte values until PC has been established.  Please #org first");

        // Note - PC gets messed up if there's an error, but that shouldn't matter because no binary
        //   gets made on an error any way.
        auto startSize = curOrgBlock.dat.size();

        for(auto& i : params)
        {
            if(i.type == DirectiveParam::Type::Integer)
            {
                auto v = i.valInt;
                if(v < 0)       v += 0x100;
                if(v < 0 || v > 0xFF)       err.error(&pos, "#byte value '" + std::to_string(i.valInt) + "' is out of range for a byte");

                curOrgBlock.dat.push_back( static_cast<u8>(v) );
            }
            else
            {
                if(tblFiles.empty())
                {
                    for(auto& c : i.valStr)
                        curOrgBlock.dat.push_back( static_cast<u8>(c) );
                }
                else
                {
                    tblFiles.back()->toBinary(err, pos, curOrgBlock.dat, i.valStr);
                }
            }
        }

        auto dif = curOrgBlock.dat.size() - startSize;
        curPC += dif;
        unbasedPC += dif;
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
            clearCurOrg();
        }

        // Start a new one
        curOrgBlock.definePos = pos;
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
        lexer.startInclude( makePath(pos, params[0].valStr), &pos );
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

    void Assembler::directive_PushTable(const Position& pos, const directiveParams_t& params)
    {
        auto tblFile = TblFile::load(err, pos, makePath(pos, params[0].valStr));
        if(tblFile)
            tblFiles.emplace_back( std::move(tblFile) );
    }

    void Assembler::directive_PopTable(const Position& pos, const directiveParams_t& params)
    {
        if(tblFiles.empty())        err.error(&pos, "#poptable:  There are no table files loaded");
        else                        tblFiles.pop_back();
    }
    
    void Assembler::directive_Pad(const Position& pos, const directiveParams_t& params)
    {
        if(!PCEstablished)  err.error(&pos, "PC has not been established. Cannot do a #pad without a preceeding #org");

        auto target = params[0].valInt;
        auto dif = target - curPC;
        if(dif < 0)         err.error(&pos, "Cannot pad to " + std::to_string(target) + " because PC is already past it (PC=" + std::to_string(curPC) + ")");

        u8 fill = 0;
        if(params.size() > 1)
        {
            auto tmp = params[1].valInt;
            if(tmp < 0)     tmp += 0x100;
            if( (tmp < 0) || (tmp > 0xFF) ) err.error(&pos, "#pad fill parameter is too large to fit in a byte");
            fill = static_cast<u8>(tmp);
        }

        curOrgBlock.dat.resize( static_cast<std::size_t>(curOrgBlock.dat.size() + dif), fill );
        unbasedPC += dif;
        curPC += dif;
    }

    void Assembler::directive_Align(const Position& pos, const directiveParams_t& params)
    {
        if(!PCEstablished)  err.error(&pos, "PC has not been established. Cannot do an #align without a preceeding #org");

        auto target = params[0].valInt;
        if(target <= 0)     err.error(&pos, "#align value must be greater than zero");
        
        u8 fill = 0;
        if(params.size() > 1)
        {
            auto tmp = params[1].valInt;
            if(tmp < 0)     tmp += 0x100;
            if( (tmp < 0) || (tmp > 0xFF) ) err.error(&pos, "#pad fill parameter is too large to fit in a byte");
            fill = static_cast<u8>(tmp);
        }

        
        auto mod = curPC % target;
        if(mod > 0)
        {
            auto dif = target - mod;
            curOrgBlock.dat.resize( static_cast<std::size_t>(curOrgBlock.dat.size() + dif), fill );
            unbasedPC += dif;
            curPC += dif;
        }
    }
}
