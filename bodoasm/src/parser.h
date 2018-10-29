#ifndef BODOASM_PARSER_H_INCLUDED
#define BODOASM_PARSER_H_INCLUDED

#include <string>
#include "error.h"
#include "lexer.h"
#include "expression.h"
#include "subparser.h"

namespace bodoasm
{
    class Assembler;
    class AsmDefinition;
    class SymbolTable;

    class Parser
    {
    public:
        static void         parse(Assembler* asmblr, Lexer* lex, AsmDefinition* def, SymbolTable* syms, ErrorReporter& er);

    private:
                            Parser(ErrorReporter& er) : err(er) {}
        Assembler*          assembler;
        Lexer*              lexer;
        AsmDefinition*      asmDefs;
        SymbolTable*        symbols;
        ErrorReporter&      err;
        
        void                fullParse();
        void                parseOne();

        bool                skipEnds(bool skipFileEnds = false);
        void                skipRemainderOfCommand();
        SubParser::Package  buildEolPackage(std::vector<Token>& owner);
        SubParser::Package  buildDirectiveParamPackage(std::vector<Token>& owner);

        Token               next();
        void                unget(const Token& t);

        void                scopeChange(const std::string& newScope);

        std::string         curScope;
        unsigned            unnamedLabelCtr = 0;
        
        void                parse_directive();
        void                parse_command();
        Expression::Ptr     parse_expression();
    };
}

#endif