#ifndef BODOASM_PARSER_H_INCLUDED
#define BODOASM_PARSER_H_INCLUDED

#include <unordered_map>
#include <string>
#include "error/error.h"
#include "lexer/lexer.h"
#include "expression/expression.h"
#include "subparser.h"
#include "types/blocktypes.h"

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

        Scope               curScope;
        unsigned            unnamedLabelCtr = 0;
        
        void                parse_directive();
        void                parse_command();
        Expression::Ptr     parse_expression();

        typedef std::vector<Token>                              tokenList_t;
        typedef std::unordered_map<std::string, tokenList_t>    subMap_t;
        std::vector<subMap_t>                   subs;               // token substitutions (ie:  macro arguments)
        std::vector<std::string>                macroScopeStack;    // empty when not expanding any macros
        std::unordered_map<std::string, Macro>  definedMacros;
        
        void                startMacroDef(const Position& pos);
        void                getStartMacroParamList(Macro& macro);
    };
}

#endif