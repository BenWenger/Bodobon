#ifndef BODOASM_PARSER_EXPRESSION_H_INCLUDED
#define BODOASM_PARSER_EXPRESSION_H_INCLUDED

#include <string>
#include "error/error.h"
#include "expression/expression.h"
#include "subparser.h"
#include "types/blocktypes.h"

namespace bodoasm
{
    class Parser_Expression : public SubParser
    {
    public:
        Parser_Expression(const Package& pkg, const Scope& scope);
        Expression::Ptr                 parse(std::size_t* count = nullptr);

    private:
        typedef Expression::Ptr         Exp;
        
        inline Exp     top_exp()        { return log_or();          }
        Exp     log_or();
        Exp     log_and();
        Exp     bin_or();
        Exp     bin_xor();
        Exp     bin_and();
        Exp     eq();
        Exp     ineq();
        Exp     shift();
        Exp     add();
        Exp     mult();
        Exp     unary();

        Exp     raw();

        void        makeBinOp(Expression::BinOp op, Exp& lhs, Exp&& rhs);
        Exp         makeUnOp(Expression::UnOp op, const Position& pos, Exp&& in);

        const Scope&        curScope;
    };
}

#endif
