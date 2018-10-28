#ifndef BODOASM_ASSEMBLER_H_INCLUDED
#define BODOASM_ASSEMBLER_H_INCLUDED

#include <string>
#include <luawrap.h>
#include "error.h"
#include "lexer.h"
#include "expression.h"
#include "symboltable.h"
#include "asmdefinition.h"


namespace bodoasm
{
    class Assembler
    {
    public:
                            Assembler(const std::string& pathToLua, const std::string& asmmode);
        void                doFile(const std::string& path);
        bool                finalizeAndOutput(const std::string& path);

    private:
        friend class Parser;
        void                defineLabel(const Position& pos, const std::string& name);
        void                defineSymbol(const Position& pos, const std::string& name, Expression::Ptr&& expr);
        void                addInstruction(const Position& pos, AddrModeMatchMap&& matches);

    private:
        ErrorReporter       err;
        Lexer               lexer;
        SymbolTable         symbols;
        AsmDefinition       asmDef;
    };
}

#endif