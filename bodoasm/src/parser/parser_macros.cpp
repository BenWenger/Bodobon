
#include "parser.h"
#include "types/blocktypes.h"
#include "symbolparse.h"
#include "symbols/symboltable.h"

namespace bodoasm
{
    void Parser::getStartMacroParamList(Macro& macro)
    {
        // sort of special case for no parameters
        skipEnds();
        Token t = next();
        if(t.str == ")")            return;
        back();

        while(true)
        {
            // skip over EOLs (this can span multiple lines)
            //   next token should be a symbol
            skipEnds();
            t = next();
            if(t.isEnd())                       err.error(&t.pos, "Unexpected EOF reached before closing parens");
            if(!t.isPossibleSymbol())           err.error(&t.pos, "Invalid name for macro parameter '" + t.str + "'");
            macro.paramNames.push_back(t.str);

            // skip ends again, next token should be either a comma (keep looping), or a close paren
            skipEnds();
            t = next();
            if(t.isEnd())                       err.error(&t.pos, "Unexpected EOF reached before closing parens");
            if(t.str == ")")                    break;
            if(t.str != ",")                    err.error(&t.pos, "Unexpected '" + t.str + "' in macro parameter list");
        }
    }

    void Parser::startMacroDef(const Position& pos)
    {
        // First is the macro name
        auto name = SymbolParse::parseDec(*this, curScope.topLabel);
        if(!name)
            err.error(&name.pos, "#macro directive first parameter must be the symbol name");

        Macro macro;
        macro.definePos = pos;

        // next token needs to be either an open paren (to indicate start of param list), or
        //   end of command (no params)
        auto t = next();
        if(t.str == "(")
        {
            getStartMacroParamList(macro);
        }
        else if(t.isEnd())
        {
            if(t.type != Token::Type::CmdEnd)   err.error(&t.pos, "Unexpected end of file reached in macro definition");
        }
        else                                    err.error(&t.pos, "Unexpected token '" + t.str + "'");

        // skip ends at the start of the macro
        skipEnds();

        // now start pulling and recording tokens until we reach EOF or #endmacro
        while(true)
        {
            t = next();
            if(t.isEoF())       err.error(&t.pos, "Unexpected EOF reached in macro definition");

            if(t.str == "#")
            {
                auto tmp = next();
                auto str = toLower(t.str);
                if(str == "macro")      err.error(&t.pos, "Cannot nest macro definitions");
                if(str == "endmacro")   break;
                back();     // tmp
            }
            macro.tokens.push_back( t );
        }

        symbols->addMacro(name.name, std::move(macro));
    }

}
