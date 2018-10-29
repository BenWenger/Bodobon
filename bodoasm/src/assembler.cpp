
#include <iomanip>
#include "assembler.h"
#include "parser.h"

namespace bodoasm
{
    bool Assembler::dirTableBuilt = false;
    Assembler::dirTable_t Assembler::dirTable;

    void Assembler::buildDirTable()
    {
        if(!dirTableBuilt)
        {
            dirTableBuilt = true;
            dirTable["org"]         = &Assembler::directive_Org;
            dirTable["include"]     = &Assembler::directive_Include;
            dirTable["rebase"]      = &Assembler::directive_Rebase;
            dirTable["byte"]        = &Assembler::directive_Byte;
        }
    }

    Assembler::Assembler(const std::string& pathToLua, const std::string& asmmode)
        : lexer(err)
        , symbols(err)
        , asmDef(err)
    {
        buildDirTable();
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
        auto i = dirTable.find(name);
        if(i == dirTable.end())
            err.fatal(nullptr, "Internal error:  directive name '" + name + "' is not found in Assembler's directive table");
        (this->*(i->second))(params);
    }

    void Assembler::directive_Byte(const directiveParams_t& params)
    {
        // TODO
        int foo = 4;
    }

    void Assembler::directive_Org(const directiveParams_t& params)
    {
        // TODO
        int foo = 4;
    }

    void Assembler::directive_Include(const directiveParams_t& params)
    {
        // TODO
        int foo = 4;
    }

    void Assembler::directive_Rebase(const directiveParams_t& params)
    {
        // TODO
        int foo = 4;
    }
}
