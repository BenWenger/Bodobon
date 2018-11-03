#ifndef BODOASM_LUAINTERFACE_H_INCLUDED
#define BODOASM_LUAINTERFACE_H_INCLUDED

#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <set>
#include "types/blocktypes.h"
#include "types/position.h"

namespace luawrap
{
    class Lua;
}

namespace bodoasm
{
    class ErrorReporter;

    class AsmDefinition
    {
    public:
        typedef std::function<void(luawrap::Lua&)>  funcSupplier_t;
        typedef std::vector<unsigned>               vec_t;

                        AsmDefinition(ErrorReporter& er);
                        ~AsmDefinition();
        void            addFuncSupplier(funcSupplier_t func)            { funcSuppliers.emplace_back(func);  }
        void            load(const std::string& path, const char* debugname = nullptr);

        vec_t           getAddrModeForMnemonic(const Position& pos, std::string& mnemonic);
        const Pattern*  getPatternForAddrMode(unsigned addrmode);

        int             guessInstructionSize(const Position& pos, const std::string& mnemonic, AddrModeMatchMap& patterns);
        int             generateBinary(const Position& pos, const std::string& mnemonic, const AddrModeMatchMap& patterns, std::vector<u8>& bin, int insertoffset = 0, int requiredsize = -1);

    private:
        ErrorReporter&  err;

        std::unique_ptr<luawrap::Lua>           lua;
        std::map<unsigned, Pattern>             addrModePatterns;   // patterns associated with addr modes
        std::unordered_map<std::string, vec_t>  mnemonicModes;      // addr modes associated with mnemonics
        std::unordered_map<std::string, vec_t>  suffixModes;        // addr modes associated with suffixes

        std::vector<funcSupplier_t>             funcSuppliers;

        void            clear();
        void            loadAddrModes();
        void            loadMnemonics()     {   loadModeMap("Mnemonics", mnemonicModes);    }
        void            loadSuffixes()      {   loadModeMap("Suffixes", suffixModes);       }

        void            loadModeMap(const std::string& name, std::unordered_map<std::string,vec_t>& mp);

        void            pushPatterns(const AddrModeMatchMap& patterns);

    };
}

#endif
