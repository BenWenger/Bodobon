#ifndef BODOASM_MACROPROCESSOR_H_INCLUDED
#define BODOASM_MACROPROCESSOR_H_INCLUDED

#include <unordered_map>
#include <string>
#include <memory>
#include "types/blocktypes.h"
#include "error/error.h"
#include "lexer/tokensource.h"

namespace bodoasm
{
    class Lexer;

    class MacroProcessor : public TokenSource
    {
    public:
                        MacroProcessor(ErrorReporter& e, Lexer* lex);

        virtual Token   next() override;
        virtual void    unget(const Token& t) override;

        void            defineMacro(const Position& definePos);
        void            setScopePointer(Scope* sc)          { scope = sc;       }
    private:
        friend class Recurse;
        class Recurse
        {
        public:         Recurse(MacroProcessor* mc, const Position& pos);
                        ~Recurse();
        private:        MacroProcessor*     obj;
        };
        ErrorReporter&  err;
        Lexer*          lexer;
        Scope*          scope;
        
        typedef std::unique_ptr<Macro>                      macroPtr_t;
        typedef std::unordered_map<std::string,macroPtr_t>  macroMap_t;
        
        void            addMacro(const std::string& name, Macro&& mac);
        const Macro*    getMacro(const std::string& name);

        macroMap_t          macros;
        int                 recursionDepth;
        static const int    maxRecursionDepth = 20;

        typedef std::unordered_map<std::string, std::vector<Token>> argList_t;
        typedef std::vector<std::vector<Token>>                     ordArgList_t;

        void            expandMacro(const Token& backtick);
        argList_t       getArgList(const std::string& macname, const Macro& macro);

        Token           nextSkipEnd();
    };

}

#endif
