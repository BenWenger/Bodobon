#ifndef BODOASM_ASSEMBLER_H_INCLUDED
#define BODOASM_ASSEMBLER_H_INCLUDED

#include <string>
#include <luawrap.h>
#include "error.h"
#include "lexer.h"
#include "expression.h"
#include "symboltable.h"
#include "asmdefinition.h"
#include "directivespecs.h"


namespace bodoasm
{
    class Assembler
    {
    public:
                            Assembler(const std::string& pathToLua, const std::string& asmmode);
        void                doFile(const std::string& path);
        bool                finalizeAndOutput(const std::string& path);

    private:
        friend class Parser;

        void                defineLabel(const Position& pos, const std::string& name);
        void                defineSymbol(const Position& pos, const std::string& name, Expression::Ptr&& expr);
        void                doInstruction(const Position& pos, const std::string& mnemonic, AddrModeMatchMap&& matches);
        void                doDirective(const Position& pos, const std::string& name, const directiveParams_t& params);

    private:
        typedef void (Assembler::*dirFunc_t)(const Position&,const directiveParams_t&);
        typedef std::unordered_map<std::string, dirFunc_t> dirTable_t;
        static bool         dirTableBuilt;
        static dirTable_t   dirTable;
        static void         buildDirTable();

        void                directive_Org       (const Position& pos, const directiveParams_t& params);
        void                directive_Include   (const Position& pos, const directiveParams_t& params);
        void                directive_Rebase    (const Position& pos, const directiveParams_t& params);
        void                directive_Endbase   (const Position& pos, const directiveParams_t& params);
        void                directive_Byte      (const Position& pos, const directiveParams_t& params);

    private:
        ErrorReporter       err;
        Lexer               lexer;
        SymbolTable         symbols;
        AsmDefinition       asmDef;
        
        struct OrgBlock
        {
            int_t               orgAddr;
            int_t               fileOffset;
            int_t               sizeCap;
            int                 fillVal;
            bool                hasSize;
            bool                hasFill;
            std::vector<u8>     dat;
        };

        struct UnfinishedCmd
        {
            unsigned            orgBlockId;
            unsigned            size;
            //  TODO more here
        };
        bool                    resolveAndTypeMatch(AddrModeMatchMap& matches, bool force);
        int                     getBinaryForInstruction(AddrModeMatchMap& matches, OrgBlock& blk, int insertPos);

        void                    addLuaFuncs(luawrap::Lua& lua);
        int                     lua_getPC(luawrap::Lua& lua);

        void                    clearCurOrgBlock();
        std::vector<OrgBlock>   orgBlocks;
        OrgBlock                curOrgBlock;
        int_t                   curPC;
        int_t                   unbasedPC;
        bool                    rebasing;
        bool                    PCEstablished;
    };
}

#endif