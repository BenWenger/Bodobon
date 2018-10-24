
#include "symboltable.h"

namespace bodoasm
{
    SymbolTable::SymbolTable(ErrorReporter& er)
        : err(er)
    {}

    void SymbolTable::addSymbol(const std::string& fullName, const Position& definePos, Expression::Ptr&& expr)
    {
        Symbol sym;
        sym.definePos = definePos;
        sym.expr = std::move(expr);

        auto iter = symbols.insert( {fullName, std::move(sym)} );
        if(!iter.second)
        {
            err.error(&definePos, fullName + " already defined here: " + err.formatPosition(iter.first->second.definePos));
        }
    }

    bool SymbolTable::isSymbolDefined(const std::string& scope, const std::string& name)
    {
        auto iter = getEntry(scope,name);
        return iter != symbols.end();
    }

    bool SymbolTable::isSymbolResolved(const std::string& scope, const std::string& name)
    {
        auto iter = getEntry(scope,name);
        if(iter == symbols.end())
            return false;
        return iter->second.expr->eval(err,*this,false);
    }
    
    /*
        Expression::Ptr     get(const std::string& scope, const std::string& name);

        void                forceResolveAll(ErrorReporter& err);

    private:
        struct Symbol
        {
            Position            definePos;
            Expression::Ptr     expr;
        };
        std::unordered_map<std::string,Expression::Ptr>     symbols;
    };*/
}
