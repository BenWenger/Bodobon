
#include "parser.h"
#include "directives/directivespecs.h"
#include "parser_expression.h"
#include "assembler/assembler.h"
#include "error/error.h"

namespace bodoasm
{
    namespace
    {
        class DirectiveTypeTraverser
        {
        public:
            typedef std::vector<DirectiveParam::Type>       vec_t;

            DirectiveTypeTraverser(const vec_t& p)
                : params(p)
                , nextPos(0)
            {
                advance();
            }

            DirectiveParam::Type peek() const { return cur;     }

            void advance()
            {
                if(nextPos >= params.size())
                    cur = DirectiveParam::Type::End;
                else
                {
                    cur = params[nextPos];
                    if(cur > DirectiveParam::Type::LoopBack)
                    {
                        nextPos -= (cur - DirectiveParam::Type::LoopBack);
                        cur = params[nextPos];
                    }
                    nextPos++;
                }
            }

        private:
            const vec_t&            params;
            std::size_t             nextPos;
            DirectiveParam::Type    cur;
        };

        const char* const getRequiredTypeName(DirectiveParam::Type t)
        {
            switch(DirectiveParam::realType(t))
            {
            case DirectiveParam::Type::Integer:       return "Integer";
            case DirectiveParam::Type::String:        return "String";
            case DirectiveParam::Type::IntOrString:   return "Integer or String";
            }

            return "<Internal error:  Unknown type>";
        }
    }

    
    SubParser::Package Parser::buildDirectiveParamPackage(std::vector<Token>& owner)
    {
        owner.clear();
        while(true)
        {
            Token t = next();
            if(t.str == "," && owner.empty())
                err.error(&t.pos, "Unexpected character ','");
            if(t.isEnd() || t.str == ",")
            {
                unget(t);
                t.type = Token::Type::CmdEnd;
                owner.push_back(t);
                break;
            }
            else
                owner.push_back(t);
        }
        
        SubParser::Package      out;
        out.errReport = &err;
        out.tokenList = owner.data();
        out.tokenListSize = owner.size();
        return out;
    }

    void Parser::parse_directive()
    {
        Token t = next();
        if(t.type != Token::Type::Misc)         err.error(&t.pos, "Expected directive name to follow '#' symbol");
        auto name = toLower(t.str);
        auto directivePos = t.pos;

        // special case for #macro and #endmacro ... the parser handles these
        if(t.str == "macro")
        {
            macroProc->defineMacro(directivePos);
            return;
        }
        else if(t.str == "endmacro")
            err.error(&t.pos, "#endmacro directive reached outside of macro definition");

        // get the directive specs
        bool isCustom = true;
        const auto* spec = asmDefs->getCustomDirectiveSpec(name);      // was it specified by the Lua?
        if(!spec)
        {
            isCustom = false;                       // guess not.  See if it's an assembler directive
            auto iter = directiveSpecs.find(name);
            if(iter != directiveSpecs.end())
                spec = &iter->second;
        }
        if(!spec)
            err.error(&t.pos, "'" + name + "' is an unrecognized directive");

        DirectiveTypeTraverser  paramTypes(*spec);
        directiveParams_t       params;

        std::vector<Token>      tokenOwner;
        while(true)
        {
            auto pkg = buildDirectiveParamPackage(tokenOwner);
            if(pkg.tokenListSize <= 1)      // EOC found, no more parameters given
                break;

            Parser_Expression p(pkg, curScope);
            auto expr = p.parse();
            expr->eval(err, *symbols, true);

            bool typematch = false;
            switch(DirectiveParam::realType(paramTypes.peek()))
            {
            case DirectiveParam::Type::Integer:
                typematch = expr->isInteger();
                break;
            case DirectiveParam::Type::String:
                typematch = expr->isString();
                break;
            case DirectiveParam::Type::IntOrString:
                typematch = expr->isInteger() || expr->isString();
                break;
            case DirectiveParam::Type::End:
                err.error(&expr->getPos(), "Too many parameters for directive '" + name + "'");
                break;
            }
            if(!typematch)
            {
                err.error(&tokenOwner[0].pos, "Directive '" + name + "' parameter " + std::to_string(params.size()+1) +
                          " should be of type " + getRequiredTypeName(paramTypes.peek()) );
            }

            DirectiveParam dp;
            if(expr->isInteger())
            {
                dp.type = DirectiveParam::Type::Integer;
                dp.valInt = expr->asInteger();
            }
            else if(expr->isString())
            {
                dp.type = DirectiveParam::Type::String;
                dp.valStr = expr->asString();
            }
            params.emplace_back( std::move(dp) );
            paramTypes.advance();

            // lastly, if there was a comma at the end, drop it
            t = next();
            if(t.str == ",")
            {
                // also, drop any and all newlines after a comma.  That way a directive can span multiple
                //   lines, as long as each line ends with a comma
                while(true)
                {
                    t = next();
                    if(t.type != Token::Type::CmdEnd)
                        break;
                }
            }
            unget(t);
        }

        // Now that we have all the inputs, make sure nothing but optionals are omitted
        if( !DirectiveParam::isTypeOptional(paramTypes.peek()) )
        {
            t = next();
            unget(t);
            err.error(&t.pos, "Not enough parameters given to directive '" + name + "'");
        }

        // Otherwise, all is well
        //   if this is conditional ... we (the parser) need to handle it.
        // Everything else goes to the assembler
        if     (name == "if")       condDirective_if(directivePos, params);
        else if(name == "elif")     condDirective_elif(directivePos, params);
        else if(name == "else")     condDirective_else(directivePos, params);
        else if(name == "endif")    condDirective_endif(directivePos, params);
        else if(isCustom)           asmDefs->doDirective(directivePos, name, params);
        else                        assembler->doDirective(directivePos, name, params);
    }

    
    void Parser::condDirective_if(const Position& pos, const directiveParams_t& params)
    {
        CondBlock blk;
        blk.state = (params[0].valInt == 0) ? CondBlock::State::NotYet : CondBlock::State::Active;
        conditionalState.push_back(blk);
    }
    
    void Parser::condDirective_elif(const Position& pos, const directiveParams_t& params)
    {
        if(conditionalState.empty())        err.error(&pos, "#elif reached without matching #if");
        auto& blk = conditionalState.back();
        if(blk.elseReached)                 err.error(&pos, "#elif reached after #else command");

        if(blk.state == CondBlock::State::NotYet)
        {
            if(params[0].valInt != 0)
                blk.state = CondBlock::State::Active;
        }
        else
            blk.state = CondBlock::State::Done;
    }
    
    void Parser::condDirective_else(const Position& pos, const directiveParams_t& params)
    {
        if(conditionalState.empty())        err.error(&pos, "#else reached without matching #if");
        auto& blk = conditionalState.back();
        if(blk.elseReached)                 err.error(&pos, "Second #else reached after #else command");

        if(blk.state == CondBlock::State::NotYet)       blk.state = CondBlock::State::Active;
        else                                            blk.state = CondBlock::State::Done;
        blk.elseReached = true;
    }
    
    void Parser::condDirective_endif(const Position& pos, const directiveParams_t& params)
    {
        if(conditionalState.empty())        err.error(&pos, "#endif reached without matching #if");
        conditionalState.pop_back();
    }

}