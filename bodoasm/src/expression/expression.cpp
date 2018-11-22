
#include "expression.h"
#include "symbols/symboltable.h"
#include "types/blocktypes.h"

namespace bodoasm
{
    namespace
    {
        const char* const unaryNames[] = {
            "?defined",
            "-",
            "!",
            "~",
            "<",
            ">",
            "^",
            "<>"
        };
        
        const char* const binaryNames[] = {
            "*",
            "/",
            "%",
            "+",
            "-",
            "<<",
            ">>",
            "<",
            "<=",
            ">",
            ">=",
            "==",
            "!=",
            "&",
            "^",
            "|",
            "&&",
            "||"
        };
        
        const char* const typeNames[] {
            "symbol",
            "integer",
            "string",
            "unary expression",
            "binary expression"
        };
    }
    
    std::string Expression::getName(UnOp op)    { return unaryNames[static_cast<int>(op)];  }
    std::string Expression::getName(BinOp op)   { return binaryNames[static_cast<int>(op)]; }
    std::string Expression::getName(Type ty)    { return typeNames[static_cast<int>(ty)];   }
    
    ///////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////

    Expression::Expression(const Position& p, int_t val)
        : pos(p)
        , type( Type::Integer )
        , valInt(val)
    {}


    Expression::Expression(const Position& p, const std::string& s, Type t)
        : pos(p)
        , type(t)
        , valStr(s)
    {}

    Expression::Expression(const Position& p, UnOp op, Ptr&& v)
        : pos(p)
        , type( Type::Unary )
        , unOp( op )
        , lhs( std::move(v) )
    {
        if(!lhs)
            throw std::runtime_error("Internal error:  null expression passed to unary expression ctor");
    }
    
    Expression::Expression(const Position& p, BinOp op, Ptr&& a, Ptr&& b)
        : pos(p)
        , type( Type::Binary )
        , binOp( op )
        , lhs( std::move(a) )
        , rhs( std::move(b) )
    {
        if(!lhs || !rhs)
            throw std::runtime_error("Internal error:  null expression passed to binary expression ctor");
    }
    
    std::string Expression::asString() const
    {
        if(!isString())     throw std::runtime_error("Internal error:  Expression::asString called when expression is not a string");
        return valStr;
    }
        
    int_t Expression::asInteger() const
    {
        if(!isInteger())    throw std::runtime_error("Internal error:  Expression::asInteger called when expression is not an integer");
        return valInt;
    }

    bool Expression::eval(ErrorReporter& err, SymbolTable& syms, bool force)
    {
        switch(type)
        {
        case Type::Integer:     return true;
        case Type::String:      return true;
        case Type::Unary:       return eval_unop(err,syms,force);
        case Type::Binary:      return eval_binop(err,syms,force);
        case Type::Symbol:      return eval_symbol(err,syms,force);
        }

        err.fatal(nullptr, "Internal error - Expression has a bad type");
        return false;
    }

    bool Expression::eval_unop(ErrorReporter& err, SymbolTable& syms, bool force)
    {
        // Special unary ops that don't evaluate their innards
        switch(unOp)
        {
        case UnOp::Defined:
            if(!lhs->isSymbol())
                err.error(&pos, getName(unOp) + " operator must be followed by a symbol name");
            valInt = syms.isSymbolDefined(lhs->valStr) ? 1 : 0;
            type = Type::Integer;
            lhs.reset();
            return true;
        }

        if(!lhs->eval(err,syms,force))          return false;

        if(lhs->isInteger())
        {
            switch(unOp)
            {
            case UnOp::Neg:     valInt = -lhs->valInt;                  break;
            case UnOp::Not:     valInt = lhs->valInt != 0;              break;
            case UnOp::BinNot:  valInt = ~lhs->valInt;                  break;
            case UnOp::Lo:      valInt = lhs->valInt & 0xFF;            break;
            case UnOp::Hi:      valInt = (lhs->valInt >> 8) & 0xFF;     break;
            case UnOp::Bank:    valInt = (lhs->valInt >> 16) & 0xFF;    break;
            case UnOp::LoWord:  valInt = lhs->valInt & 0xFFFF;          break;
            default:
                err.error(&pos, getName(unOp) + " unary operator cannot be used on integers");
            }
            type = Type::Integer;
            lhs.reset();
            return true;
        }
        if(lhs->isString())
        {
            err.error(&pos, getName(unOp) + " unary operator cannot be used on strings");
        }
        err.fatal(&pos, "Internal error:  Expression::eval_unop bad point reached");
        return false;
    }
    
    bool Expression::eval_binop(ErrorReporter& err, SymbolTable& syms, bool force)
    {
        // we want to eval both left and right -- no short circuiting!
        bool ok = lhs->eval(err,syms,force);
        ok      = rhs->eval(err,syms,force) && ok;
        if(!ok)                             return false;

        // are there type differences?
        if(lhs->type != rhs->type)
        {
            // currently there are no operators that blend types, and no type conversions,
            //   so this is an error
            err.error(&pos, getName(binOp) + " binary operator cannot operate on differing types.  Left side is " + getName(lhs->type) + ", Right side is " + getName(rhs->type));
        }

        if(lhs->isString())
        {
            // only supported operators on strings are +, ==, !=
            switch(binOp)
            {
            case BinOp::Add:
                valStr = lhs->valStr + rhs->valStr;
                type = Type::String;
                break;
            case BinOp::Eq:
                valInt = (lhs->valStr == rhs->valStr);
                type = Type::Integer;
                break;
            case BinOp::NotEq:
                valInt = (lhs->valStr != rhs->valStr);
                type = Type::Integer;
                break;
            default:
                err.error(&pos, getName(binOp) + " binary operator cannot be used on strings.");
            }

            pos = lhs->pos;
            lhs.reset();
            rhs.reset();
            return true;
        }
        if(lhs->isInteger())
        {
            switch(binOp)
            {
            case BinOp::Mul:        valInt = lhs->valInt *  rhs->valInt;    break;
            case BinOp::Add:        valInt = lhs->valInt +  rhs->valInt;    break;
            case BinOp::Sub:        valInt = lhs->valInt -  rhs->valInt;    break;
            case BinOp::Less:       valInt = lhs->valInt <  rhs->valInt;    break;
            case BinOp::LeEq:       valInt = lhs->valInt <= rhs->valInt;    break;
            case BinOp::Greater:    valInt = lhs->valInt >  rhs->valInt;    break;
            case BinOp::GrEq:       valInt = lhs->valInt >= rhs->valInt;    break;
            case BinOp::Eq:         valInt = lhs->valInt == rhs->valInt;    break;
            case BinOp::NotEq:      valInt = lhs->valInt != rhs->valInt;    break;
            case BinOp::BinAnd:     valInt = lhs->valInt &  rhs->valInt;    break;
            case BinOp::BinXor:     valInt = lhs->valInt ^  rhs->valInt;    break;
            case BinOp::BinOr:      valInt = lhs->valInt |  rhs->valInt;    break;
            case BinOp::LogAnd:     valInt = lhs->valInt && rhs->valInt;    break;
            case BinOp::LogOr:      valInt = lhs->valInt || rhs->valInt;    break;
            case BinOp::Div:
                if(rhs->valInt == 0)    err.error(&pos, "Division by zero");
                valInt = lhs->valInt / rhs->valInt;
                break;
            case BinOp::Mod:
                if(rhs->valInt == 0)    err.error(&pos, "Division by zero");
                valInt = lhs->valInt % rhs->valInt;
                break;
            case BinOp::LShift:
                if(rhs->valInt < 0)     valInt = lhs->valInt >> -rhs->valInt;
                else                    valInt = lhs->valInt << rhs->valInt;
                break;
            case BinOp::RShift:
                if(rhs->valInt < 0)     valInt = lhs->valInt << -rhs->valInt;
                else                    valInt = lhs->valInt >> rhs->valInt;
                break;

            default:
                err.error(&pos, getName(binOp) + " binary operator cannot be used on integers");
            }
            pos = lhs->pos;
            type = Type::Integer;
            lhs.reset();
            rhs.reset();
            return true;
        }
        
        err.fatal(&pos, "Internal error:  Expression::eval_binop bad point reached");
        return false;
    }
    
    bool Expression::eval_symbol(ErrorReporter& err, SymbolTable& syms, bool force)
    {
        auto expr = syms.getSymbol(valStr);
        if(expr && expr->isResolved())
        {
            type = expr->type;
            valInt = expr->valInt;
            valStr = expr->valStr;
            return true;
        }
        else if(force)
        {
            if(syms.isSymbolDefined(valStr))
                err.error(&pos, "Unable to resolve symbol '" + valStr + "'");
            else
            {
                if(Scope::isNamelessName(valStr))       err.error(&pos, "Nameless label reference is out of bounds");
                else                                    err.error(&pos, "'" + valStr + "' is undefined");
            }
        }
        return false;
    }
}

