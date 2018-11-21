#ifndef BODOASM_ASSEMBLER_H_INCLUDED
#define BODOASM_ASSEMBLER_H_INCLUDED

#include <unordered_map>
#include <string>
#include <luawrap.h>
#include <dshfs.h>
#include "error/error.h"
#include "lexer/lexer.h"
#include "expression/expression.h"
#include "symbols/symboltable.h"
#include "asmdefs/asmdefinition.h"
#include "directives/directivespecs.h"
#include "macros/macroprocessor.h"
#include "tbl/tblfile.h"


namespace bodoasm
{
    class Assembler
    {
    public:
                            Assembler(const std::string& pathToLua, const std::string& asmmode);
        void                assembleFile(const std::string& path);
        void                finalize();
        bool                okToProceed();
        void                output(dshfs::File::Ptr& file);
        
        unsigned            getErrCount() const         { return err.getErrCount();         }
        unsigned            getWarnCount() const        { return err.getWarningCount();     }

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
        void                directive_PushTable (const Position& pos, const directiveParams_t& params);
        void                directive_PopTable  (const Position& pos, const directiveParams_t& params);
        void                directive_Pad       (const Position& pos, const directiveParams_t& params);
        void                directive_Align     (const Position& pos, const directiveParams_t& params);

    private:
        ErrorReporter       err;
        Lexer               lexer;
        SymbolTable         symbols;
        AsmDefinition       asmDef;
        MacroProcessor      macroProc;
        
        struct StateEntry
        {
            enum class Type
            {
                Integer,
                String
            };
            Type            type;
            lua_Integer     valInt;
            std::string     valStr;
        };
        typedef std::unordered_map<std::string, StateEntry>     luaStateMap_t;

        struct Future
        {
            Position            pos;
            int                 binaryPos;
            unsigned            mnemonic;
            AddrModeMatchMap    matches;
            int                 promisedSize;
            int_t               pcAtTime;
            luaStateMap_t       stateMap;
        };
        
        struct OrgBlock
        {
            Position            definePos;
            int_t               orgAddr;
            int_t               fileOffset;
            int_t               sizeCap;
            int                 fillVal;
            bool                hasSize;
            bool                hasFill;
            std::vector<u8>     dat;
            std::vector<Future> futures;
        };
        bool                    resolveAndTypeMatch(AddrModeMatchMap& matches, bool force);
        void                    resolveFutures();
        void                    checkOrgConflicts();

        void                    addLuaFuncs(luawrap::Lua& lua);
        int                     lua_getPC(luawrap::Lua& lua);
        int                     lua_get(luawrap::Lua& lua);
        int                     lua_set(luawrap::Lua& lua);

        void                    clearCurOrg();
        std::vector<OrgBlock>   orgBlocks;
        OrgBlock                curOrgBlock;
        int_t                   curPC;
        int_t                   unbasedPC;
        bool                    rebasing;
        bool                    PCEstablished;
        luaStateMap_t           curLuaState;

        std::vector<TblFile::Ptr>   tblFiles;
    };
}

#endif