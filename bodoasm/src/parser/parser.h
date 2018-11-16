#ifndef BODOASM_PARSER_H_INCLUDED
#define BODOASM_PARSER_H_INCLUDED

#include <unordered_map>
#include <string>
#include <vector>
#include "error/error.h"
#include "lexer/lexer.h"
#include "expression/expression.h"
#include "subparser.h"
#include "types/blocktypes.h"
#include "lexer/tokensource.h"
#include "macros/macroprocessor.h"
#include "types/position.h"
#include "directives/directivespecs.h"

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
        void                parseOneOffCondition();

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

        struct CondBlock
        {
            enum class State
            {
                NotYet,     // we've seen "#if 0"s but no "#if 1" yet
                Active,     // We are currently in an "#if 1"
                Done,       // We have already exited an "#if 1", and are now in #elif/else .. waiting for #endif
                Disabled    // the entire elif chain is disabled because of an outer elif chain
            };
            State       state;
            bool        elseReached = false;
        };
        std::vector<CondBlock>      conditionalState;
        void                condDirective_if   (const Position& pos, const directiveParams_t& params);
        void                condDirective_elif (const Position& pos, const directiveParams_t& params);
        void                condDirective_else (const Position& pos, const directiveParams_t& params);
        void                condDirective_endif(const Position& pos, const directiveParams_t& params);
    };
}

#endif