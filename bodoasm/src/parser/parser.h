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
#include "macros/macroprocessor.h"

namespace bodoasm
{
    class Assembler;
    class AsmDefinition;
    class SymbolTable;
    class MacroProcessor;

    class Parser : private TokenSource
    {
    public:
        static void         parse(Assembler* asmblr, Lexer* lex, MacroProcessor* mac, AsmDefinition* def, SymbolTable* syms, ErrorReporter& er);

    private:
                            Parser(ErrorReporter& er) : err(er) {}
        Assembler*          assembler;
        Lexer*              lexer;
        AsmDefinition*      asmDefs;
        SymbolTable*        symbols;
        MacroProcessor*     macroProc;
        ErrorReporter&      err;
        
        void                fullParse();
        void                parseOne();

        bool                skipEnds(bool skipFileEnds = false);
        void                skipRemainderOfCommand();
        SubParser::Package  buildEolPackage(std::vector<Token>& owner);
        SubParser::Package  buildDirectiveParamPackage(std::vector<Token>& owner);
        
        virtual Token       next() override                 { return macroProc->next();     }
        virtual void        unget(const Token& t) override  { return macroProc->unget(t);   }

        Scope               curScope;
        
        void                parse_directive();
        void                parse_command();
        Expression::Ptr     parse_expression();

        // TODO -- next thing is if/elif/else directives.  Those have to be handled on parser
        //   level because they disrupt parsing logic.
    };
}

#endif