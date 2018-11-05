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

    class Parser : private BufferedTokenSource
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


        struct MacroPlayback
        {
            typedef std::unique_ptr<MacroPlayback>      Ptr;

            Position::Ptr       invokePos;
            const Macro*        mac;
            std::size_t         tokenPos;       // index of next token to be fetched from mac's token list

            typedef std::unordered_map<std::string, std::vector<Token>>     args_t;
            typedef std::vector<Token>::const_iterator                      argIter_t;
            bool                outputtingArg;  // true if currently expanding one of the arguments, rather than the macro itself
            args_t              arguments;
            argIter_t           argIter;        // if outputtingArg is true, this is the next arg to be output
            argIter_t           argIterEnd;     // this is where arg outputting ends

            // I'm holding onto iterators, so these objects cannot be moved or copied
            MacroPlayback() = default;
            MacroPlayback(const MacroPlayback&) = delete;
            MacroPlayback& operator = (const MacroPlayback&) = delete;
            MacroPlayback(MacroPlayback&&) = delete;
            MacroPlayback& operator = (MacroPlayback&&) = delete;
        };
        std::vector<MacroPlayback::Ptr>                 macroStack;
        
        Token               fetchMacroToken();
        Token               invokeMacro(const Token& backtick);
        Token               nestTokenInMacro(Token t, const MacroPlayback& mpb);
    };
}

#endif