
#include <iomanip>
#include "assembler.h"
#include "parser.h"

namespace bodoasm
{
    Assembler::Assembler(const std::string& pathToLua, const std::string& asmmode)
        : lexer(err)
        , symbols(err)
        , asmDef(err)
    {
        asmDef.load(pathToLua + asmmode + ".lua", (asmmode + ".lua").c_str() );
    }
    
    void Assembler::doFile(const std::string& path)
    {
        lexer.startFile(path);
        Parser::parse(this, &lexer, &asmDef, &symbols, err);
    }
    
    void Assembler::defineLabel(const Position& pos, const std::string& name)
    {
        // TODO symbols.addSymbol(name, pos, Expression::buildInt(pos, pc) );
    }

    void Assembler::defineSymbol(const Position& pos, const std::string& name, Expression::Ptr&& expr)
    {
        expr->eval(err, symbols, false);
        symbols.addSymbol(name, pos, std::move(expr));
    }

    void Assembler::addInstruction(const Position& pos, AddrModeMatchMap&& matches)
    {
        int foo = 4;
    }

    void Assembler::doDirective(const std::string& name, const directiveParams_t& params)
    {
        int bar = 4;
    }
}
