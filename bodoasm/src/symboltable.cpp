
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

    void SymbolTable::addIncompleteSymbol(const std::string& fullName, const Position& definePos)
    {
        Symbol sym;
        sym.definePos = definePos;
        auto iter = symbols.insert( {fullName, std::move(sym)} );
        if(!iter.second)
        {
            err.error(&definePos, fullName + " already defined here: " + err.formatPosition(iter.first->second.definePos));
        }
    }
    
    void SymbolTable::changeSymbolValue(const std::string& scope, const std::string& name, Expression::Ptr&& expr)
    {
        auto i = getEntry(scope, name);
        if(i == symbols.end())
            err.fatal(nullptr, "Internal error when trying to update symbol value (scope='" + scope + "' name='" + name + "'");
        i->second.expr = std::move(expr);
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
        return resolveSymbol(iter, false);
    }
    
    Expression* SymbolTable::get(const std::string& scope, const std::string& name)
    {
        auto iter = getEntry(scope,name);
        if(iter == symbols.end())       return nullptr;
        resolveSymbol(iter, false);
        return iter->second.expr.get();
    }
    
    auto SymbolTable::getEntry(const std::string& scope, const std::string& name) -> map_t::iterator
    {
        // Is the symbol explicitly fully qualified?  (easy mode)
        if(name.substr(0,1) == "-")
        {
            return symbols.find( name.substr(1) );
        }

        // Otherwise, start at our current scope and move up tiers until we find the symbol

        // for local symbols, the connector is '.', otherwise it's '-'
        std::string connector;
        if(name.substr(0,1) != ".")
            connector = "-";

        auto sc = scope;
        auto end = symbols.end();
        auto iter = end;
        while( true )
        {
            iter = symbols.find( sc + name );
            if(iter != end)     break;              // found it!

            auto pos = sc.rfind('-');
            if(pos == 0 || pos == sc.npos) break;   // no more scopes to run down!
            sc = sc.substr(0,pos);
        }
        return iter;
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
