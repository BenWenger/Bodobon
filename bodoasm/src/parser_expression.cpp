
#include "parser_expression.h"

namespace bodoasm
{
    Parser_Expression::Parser_Expression(const Package& pkg, const std::string& scope)
        : SubParser(pkg, 20)
        , curScope(scope)
    {}
    
    Expression::Ptr Parser_Expression::parse(std::size_t* count)
    {
        auto out = top_exp();
        if(count)       *count = getCurrentLexPos();
        return out;
    }

    auto Parser_Expression::log_or() -> Exp
    {
        Exp exp = log_and();
        while(true)
        {
            auto t = next();
            if     (t.str == "||")  makeBinOp( Expression::BinOp::LogOr, exp, log_and() );
            else                    break;
        }
        back();
        return exp;
    }
    
    auto Parser_Expression::log_and() -> Exp
    {
        Exp exp = bin_or();
        while(true)
        {
            auto t = next();
            if     (t.str == "&&")  makeBinOp( Expression::BinOp::LogAnd, exp, bin_or() );
            else                    break;
        }
        back();
        return exp;
    }
    
    auto Parser_Expression::bin_or() -> Exp
    {
        Exp exp = bin_xor();
        while(true)
        {
            auto t = next();
            if     (t.str == "|")   makeBinOp( Expression::BinOp::BinOr, exp, bin_xor() );
            else                    break;
        }
        back();
        return exp;
    }
    
    auto Parser_Expression::bin_xor() -> Exp
    {
        Exp exp = bin_and();
        while(true)
        {
            auto t = next();
            if     (t.str == "^")   makeBinOp( Expression::BinOp::BinXor, exp, bin_and() );
            else                    break;
        }
        back();
        return exp;
    }
    
    auto Parser_Expression::bin_and() -> Exp
    {
        Exp exp = eq();
        while(true)
        {
            auto t = next();
            if     (t.str == "&")   makeBinOp( Expression::BinOp::BinAnd, exp, eq() );
            else                    break;
        }
        back();
        return exp;
    }
    
    auto Parser_Expression::eq() -> Exp
    {
        Exp exp = ineq();
        while(true)
        {
            auto t = next();
            if     (t.str == "==")  makeBinOp( Expression::BinOp::Eq,    exp, ineq() );
            else if(t.str == "!=")  makeBinOp( Expression::BinOp::NotEq, exp, ineq() );
            else                    break;
        }
        back();
        return exp;
    }
    
    auto Parser_Expression::ineq() -> Exp
    {
        Exp exp = shift();
        while(true)
        {
            auto t = next();
            if     (t.str == ">=")  makeBinOp( Expression::BinOp::GrEq,    exp, shift() );
            else if(t.str == ">")   makeBinOp( Expression::BinOp::Greater, exp, shift() );
            else if(t.str == "<=")  makeBinOp( Expression::BinOp::LeEq,    exp, shift() );
            else if(t.str == "<")   makeBinOp( Expression::BinOp::Less,    exp, shift() );
            else                    break;
        }
        back();
        return exp;
    }
    
    auto Parser_Expression::shift() -> Exp
    {
        Exp exp = add();
        while(true)
        {
            auto t = next();
            if     (t.str == ">>")  makeBinOp( Expression::BinOp::RShift, exp, add() );
            else if(t.str == "<<")  makeBinOp( Expression::BinOp::LShift, exp, add() );
            else                    break;
        }
        back();
        return exp;
    }
    
    auto Parser_Expression::add() -> Exp
    {
        Exp exp = mult();
        while(true)
        {
            auto t = next();
            if     (t.str == "+")   makeBinOp( Expression::BinOp::Add, exp, mult() );
            else if(t.str == "-")   makeBinOp( Expression::BinOp::Sub, exp, mult() );
            else                    break;
        }
        back();
        return exp;
    }
    
    auto Parser_Expression::mult() -> Exp
    {
        Exp exp = unary();
        while(true)
        {
            auto t = next();
            if     (t.str == "*")   makeBinOp( Expression::BinOp::Mul, exp, unary() );
            else if(t.str == "/")   makeBinOp( Expression::BinOp::Div, exp, unary() );
            else if(t.str == "%")   makeBinOp( Expression::BinOp::Mod, exp, unary() );
            else                    break;
        }
        back();
        return exp;
    }
    
    auto Parser_Expression::unary() -> Exp
    {
        {
            auto t = next();
            RecursionMarker mk(this, &t.pos);
            
            if(t.str == "-")        return makeUnOp( Expression::UnOp::Neg,     t.pos, unary() );
            else if(t.str == "!")   return makeUnOp( Expression::UnOp::Not,     t.pos, unary() );
            else if(t.str == "~")   return makeUnOp( Expression::UnOp::BinNot,  t.pos, unary() );
            else if(t.str == "<")   return makeUnOp( Expression::UnOp::Lo,      t.pos, unary() );
            else if(t.str == ">")   return makeUnOp( Expression::UnOp::Hi,      t.pos, unary() );
            else if(t.str == "^")   return makeUnOp( Expression::UnOp::Bank,    t.pos, unary() );
            else if(t.str == "<>")  return makeUnOp( Expression::UnOp::LoWord,  t.pos, unary() );
            else if(t.str == "?" && !t.ws_after)
            {
                auto s = toLower(next().str);
                if(s == "defined")  return makeUnOp( Expression::UnOp::Defined, t.pos, unary() );
                else
                {
                    back();     // unget the word
                    back();     // unget the ?
                }
            }
            else
                back();
        }

        return raw();
    }

    auto Parser_Expression::raw() -> Exp
    {
        Exp exp;

        // "raw" expressions are either:
        //  - parenthetical
        //  - numeric literal
        //  - string literal
        //  - symbol name
        auto t = next();
        if(t.str == "(")        // parenthetical statement
        {
            RecursionMarker mk(this, &t.pos);
            exp = top_exp();
            t = next();
            if(t.str != ")")
                error(&t.pos,"Missing expected closing parenthesis");
        }
        else if(t.str == ".")   // possible local symbol name
        {
            auto sym = next().resolve(*this);
            if(sym.type != Token::Type::Symbol)
                error(&t.pos, "Unexpected character '.'");

            // it IS a local symbol name!
            exp = Expression::buildSymbol(t.pos, curScope + "." + sym.str);
        }
        else if(t.str == "%")   // binary literal
        {
            if(t.ws_after)  error(&t.pos, "Unexpected character '%'");
            auto val = next().resolve(*this, 2);
            if(val.type != Token::Type::Integer)    error(&val.pos, "Expected binary literal to follow '%' character");
            exp = Expression::buildInt(t.pos, val.val);
        }
        else if(t.str == "$")   // hex literal
        {
            if(t.ws_after)  error(&t.pos, "Unexpected character '$'");
            auto val = next().resolve(*this, 16);
            if(val.type != Token::Type::Integer)    error(&val.pos, "Expected hexadecimal literal to follow '$' character");
            exp = Expression::buildInt(t.pos, val.val);
        }
        else if(t.type == Token::Type::String)  // string literal
        {
            exp = Expression::buildStr(t.pos, t.strVal);
        }
        else if(t.type == Token::Type::Misc)    // global symbol name, fully qualified symbol name, or decimal numeric literal
        {
            t = t.resolve(*this);
            if(t.type == Token::Type::Integer)
                exp = Expression::buildInt( t.pos, t.val );
            else if(t.type == Token::Type::Symbol)
            {
                // global symbol name or fully qualified name
                auto dot = next();
                if(!t.ws_after && dot.str == ".")   // looks like a fully qualified name
                {
                    auto lcl = next().resolve(*this);
                    if(!dot.ws_after && lcl.type == Token::Type::Symbol)    // IS a fully qualified name!
                        exp = Expression::buildSymbol(t.pos, t.str + "." + lcl.str);
                    else
                        error(&lcl.pos, "Expected local symbol name to follow '.'");
                }
                else                                // Not a fully qualified name, just a global symbol
                {
                    back();     // unget the dot
                    exp = Expression::buildSymbol(t.pos, t.str);
                }
            }
            else
                error(&t.pos, "Internal Error:  Token::resolve did not produce an integer or symbol");
        }

        return exp;
    }
    
    void Parser_Expression::makeBinOp(Expression::BinOp op, Exp& lhs, Exp&& rhs)
    {
        lhs = Expression::buildOp(lhs->getPos(), op, std::move(lhs), std::move(rhs));
    }
    
    auto Parser_Expression::makeUnOp(Expression::UnOp op, const Position& pos, Exp&& in) -> Exp
    {
        return Expression::buildOp( pos, op, std::move(in) );
    }
}
