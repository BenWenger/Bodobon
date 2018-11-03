#ifndef BODOASM_EXPRESSION_H_INCLUDED
#define BODOASM_EXPRESSION_H_INCLUDED

#include <memory>
#include "types/basetypes.h"
#include "types/token.h"

// there might be a better way to organize this....

namespace bodoasm
{
    class ErrorReporter;
    class SymbolTable;

    class Expression
    {
    public:
        typedef std::shared_ptr<Expression>     Ptr;
        enum class UnOp 
        {
            Defined,        // ?defined
            Neg,            // -
            Not,            // !
            BinNot,         // ~
            Lo,             // <
            Hi,             // >
            Bank,           // ^
            LoWord          // <>
        };

        enum class BinOp
        {
            Mul,            // *
            Div,            // /
            Mod,            // %
            Add,            // +
            Sub,            // -
            LShift,         // <<
            RShift,         // >>
            Less,           // <
            LeEq,           // <=
            Greater,        // >
            GrEq,           // >=
            Eq,             // ==
            NotEq,          // !=
            BinAnd,         // &
            BinXor,         // ^
            BinOr,          // |
            LogAnd,         // &&
            LogOr           // ||
        };

        inline bool isInteger() const       { return type == Type::Integer;     }
        inline bool isString() const        { return type == Type::String;      }
        inline bool isResolved() const      { return isInteger() || isString(); }
        inline bool isSymbol() const        { return type == Type::Symbol;      }
        
        std::string         asString() const;
        int_t               asInteger() const;
        Position            getPos() const  { return pos;                       }

        bool                eval(ErrorReporter& err, SymbolTable& syms, bool force);
        
        inline static Ptr   buildInt(const Position& pos, int_t val)                    { return Ptr( new Expression(pos, val) );                               }
        inline static Ptr   buildStr(const Position& pos, const std::string& str)       { return Ptr( new Expression(pos, str, Type::String) );                 }
        inline static Ptr   buildSymbol(const Position& pos, const std::string& name)   { return Ptr( new Expression(pos, name, Type::Symbol) );                }
        inline static Ptr   buildOp(const Position& pos, UnOp op, Ptr&& v)              { return Ptr( new Expression(pos, op, std::move(v)) );                  }
        inline static Ptr   buildOp(const Position& pos, BinOp op, Ptr&& a, Ptr&& b)    { return Ptr( new Expression(pos, op, std::move(a), std::move(b)) );    }

    private:
        enum class Type
        {
            Symbol,
            Integer,
            String,
            Unary,
            Binary
        };

        Expression () = delete;
        Expression(const Expression&) = delete;
        Expression& operator = (const Expression&) = delete;
        Expression(Expression&&) = delete;
        Expression& operator = (Expression&&) = delete;
        
        Expression(const Position& p, int_t val);
        Expression(const Position& p, const std::string& s, Type t);
        Expression(const Position& p, UnOp op, Ptr&& v);
        Expression(const Position& p, BinOp op, Ptr&& a, Ptr&& b);
        
        bool                eval_unop(ErrorReporter& err, SymbolTable& syms, bool force);
        bool                eval_binop(ErrorReporter& err, SymbolTable& syms, bool force);
        bool                eval_symbol(ErrorReporter& err, SymbolTable& syms, bool force);
        
        
        static std::string  getName(UnOp op);
        static std::string  getName(BinOp op);
        static std::string  getName(Type ty);

        Position        pos;
        Type            type;
        BinOp           binOp;
        UnOp            unOp;
        Ptr             lhs;
        Ptr             rhs;
        int_t           valInt;
        std::string     valStr;
    };
}

#endif

