#ifndef BODOASM_SYMBOLTABLE_H_INCLUDED
#define BODOASM_SYMBOLTABLE_H_INCLUDED

#include <string>
#include <unordered_map>
#include "expression.h"
#include "position.h"
#include "error.h"

namespace bodoasm
{
    class SymbolTable
    {
    public:
                            SymbolTable(ErrorReporter& er);
        void                addSymbol(const std::string& fullName, const Position& definePos, Expression::Ptr&& expr);
        bool                isSymbolDefined(const std::string& scope, const std::string& name);
        
        void                addIncompleteSymbol(const std::string& fullName, const Position& definePos);
        void                changeSymbolValue(const std::string& scope, const std::string& name, Expression::Ptr&& expr);

        bool                isSymbolResolved(const std::string& scope, const std::string& name);
        Expression*         get(const std::string& scope, const std::string& name);

        void                forceResolveAll();

    private:
        struct Symbol
        {
            Position            definePos;
            Expression::Ptr     expr;           // may be null if no expression yet (in the case of an incomplete label)
        };
        typedef std::unordered_map<std::string,Symbol>      map_t;
        typedef std::unordered_map<std::string,Position>    guard_t;

        bool                resolveSymbol(const map_t::iterator& iter, bool force);

        map_t::iterator     getEntry(const std::string& scope, const std::string& name);
        ErrorReporter&      err;
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
