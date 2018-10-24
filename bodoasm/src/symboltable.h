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

        bool                isSymbolResolved(const std::string& scope, const std::string& name);
        Expression::Ptr     get(const std::string& scope, const std::string& name);

        void                forceResolveAll();

    private:
        struct Symbol
        {
            Position            definePos;
            Expression::Ptr     expr;
        };
        typedef std::unordered_map<std::string,Symbol> map_t;

        map_t::iterator     getEntry(const std::string& scope, const std::string& name);
        ErrorReporter&      err;
        map_t               symbols;
    };
}

#endif
