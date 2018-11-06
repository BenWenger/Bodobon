#ifndef BODOASM_PARSER_H_INCLUDED
#define BODOASM_PARSER_H_INCLUDED

#include <unordered_map>
#include <string>
#include "error/error.h"
#include "lexer/lexer.h"
#include "expression/expression.h"
#include "subparser.h"
#include "types/blocktypes.h"
#include "lexer/tokensource.h"

namespace bodoasm
{
    class Assembler;
    class AsmDefinition;
    class SymbolTable;

    class Parser : private TokenSource
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
        
        virtual Token       fetchToken() override;

        Scope               curScope;
        
        void                parse_directive();
        void                parse_command();
        Expression::Ptr     parse_expression();
        

        void                startMacroDef(const Position& pos);
        void                getStartMacroParamList(Macro& macro);


        struct MacroInvocation
        {
            const Macro*                                            macro;
            std::unordered_map< std::string, std::vector<Token> >   args;
            std::shared_ptr<Position>                               invokePos;
        };
        
        void                invokeMacro(const Token& backtick);
        MacroInvocation     buildMacroInvocation(TokenSource& src, const Token& backtick) const;
    };
}

#endif