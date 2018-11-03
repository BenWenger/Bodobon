
#include "symboltable.h"

namespace bodoasm
{
    SymbolTable::ResolveLoopGuard::ResolveLoopGuard(ErrorReporter& err, guard_t& m, const map_t::iterator& i)
        : map(m)
        , str(i->first)
    {
        auto res = map.insert({str, i->second.definePos});

        if(!res.second)     // insert failed!  That means we're already trying to resolve this!
        {
            err.error( &res.first->second, "Infinite dependency loop when trying to resolve symbol '" + str + "'" );
        }
    }

    SymbolTable::ResolveLoopGuard::~ResolveLoopGuard()
    {
        map.erase(str);
    }



    SymbolTable::SymbolTable(ErrorReporter& er)
        : err(er)
    {}

    void SymbolTable::addSymbol(const std::string& name, const Position& definePos, Expression::Ptr&& expr)
    {
        Symbol sym;
        sym.definePos = definePos;
        sym.expr = std::move(expr);

        auto iter = symbols.insert( {name, std::move(sym)} );
        if(!iter.second)
        {
            err.error(&definePos, name + " already defined here: " + err.formatPosition(iter.first->second.definePos));
        }
    }

    void SymbolTable::addIncompleteSymbol(const std::string& name, const Position& definePos)
    {
        Symbol sym;
        sym.definePos = definePos;
        auto iter = symbols.insert( {name, std::move(sym)} );
        if(!iter.second)
        {
            err.error(&definePos, name + " already defined here: " + err.formatPosition(iter.first->second.definePos));
        }
    }
    
    void SymbolTable::changeSymbolValue(const std::string& name, Expression::Ptr&& expr)
    {
        auto i = symbols.find(name);
        if(i == symbols.end())
            err.fatal(nullptr, "Internal error when trying to update symbol value ('" + name + "'");        // TODO will I need to use this function any more?
        i->second.expr = std::move(expr);
    }

    bool SymbolTable::isSymbolDefined(const std::string& name)
    {
        auto iter = symbols.find(name);
        return iter != symbols.end();
    }

    bool SymbolTable::isSymbolResolved(const std::string& name)
    {
        auto iter = symbols.find(name);
        if(iter == symbols.end())
            return false;
        return resolveSymbol(iter, false);
    }
    
    Expression* SymbolTable::get(const std::string& name)
    {
        auto iter = symbols.find(name);
        if(iter == symbols.end())       return nullptr;
        resolveSymbol(iter, false);
        return iter->second.expr.get();
    }
    
    bool SymbolTable::resolveSymbol(const map_t::iterator& iter, bool force)
    {
        ResolveLoopGuard guard(err, loopGuard, iter);
        auto& expr = iter->second.expr;
        if(!expr)
        {
            if(force)
                err.error(&iter->second.definePos, "Unable to resolve incomplete symbol '" + iter->first + "'");
            else
                return false;
        }
        return iter->second.expr->eval( err, *this, force );
    }

    void SymbolTable::forceResolveAll()
    {
        for(auto i = symbols.begin(); i != symbols.end(); ++i)
        {
            resolveSymbol(i, true);
        }
    }
}
