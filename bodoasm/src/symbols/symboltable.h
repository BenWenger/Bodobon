#ifndef BODOASM_SYMBOLTABLE_H_INCLUDED
#define BODOASM_SYMBOLTABLE_H_INCLUDED

#include <string>
#include <unordered_map>
#include "expression/expression.h"
#include "types/position.h"
#include "error/error.h"
#include "types/blocktypes.h"

namespace bodoasm
{
    class SymbolTable
    {
    public:
                            SymbolTable(ErrorReporter& er);
        void                addSymbol(const std::string& name, const Position& definePos, Expression::Ptr&& expr);
        bool                isSymbolDefined(const std::string& name);

        bool                isSymbolResolved(const std::string& name);
        Expression*         getSymbol(const std::string& name);

        void                forceResolveAll();

        // Macros
        void                addMacro(const std::string& name, Macro&& mac);
        const Macro*        getMacro(const std::string& name);

    private:
        struct Symbol
        {
            Position            definePos;
            Expression::Ptr     expr;           // may be null if no expression yet (in the case of an incomplete label)
        };
        typedef std::unique_ptr<Macro>                      macroPtr_t;
        typedef std::unordered_map<std::string,Symbol>      map_t;
        typedef std::unordered_map<std::string,macroPtr_t>  macroMap_t;
        typedef std::unordered_map<std::string,Position>    guard_t;

        bool                resolveSymbol(const map_t::iterator& iter, bool force);

        ErrorReporter&      err;
        macroMap_t          macros;
        map_t               symbols;
        guard_t             loopGuard;

        class ResolveLoopGuard
        {
        public:
            ResolveLoopGuard(ErrorReporter& err, guard_t& m, const map_t::iterator& i);
            ~ResolveLoopGuard();

        private:
            guard_t&        map;
            std::string     str;
        };
    };
}

#endif
