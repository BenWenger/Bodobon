#ifndef BODOASM_LEXER_H_INCLUDED
#define BODOASM_LEXER_H_INCLUDED

#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include <dshfs.h>
#include "types/position.h"
#include "error/error.h"
#include "types/basetypes.h"
#include "types/token.h"
#include "tokensource.h"

namespace bodoasm
{
    class Lexer : public TokenSource
    {
    public:
                        Lexer(ErrorReporter& er);
        
        void            startFile(const std::string& filename);
        void            startInclude(const std::string& filename, const Position* errpos);

    private:
        virtual Token   fetchToken() override;

    private:
        typedef std::unique_ptr<dshfs::FileStream>  file_t;
        struct State
        {
            Position            pos;
            std::string         text;
            file_t              file;

            void clear()
            {
                pos.fileId = 0;
                pos.lineNo = 0;
                pos.linePos = 0;
                text.clear();
                file.reset();
            }
        };
        ErrorReporter&          err;
        std::vector<State>      includeStack;
        State                   cur;
        bool                    inputDone;

        bool                    getNextLine();

        bool                    skipToNextToken(Token& tok);
        bool                    onSkippableChar() const;
        bool                    onWhitespaceChar() const;
        bool                    eol() const;
        char                    peek() const;
        char                    advance();
        void                    skipWhitespace();


        void                    lexStringLiteral(Token& tok, char closer);
        void                    lexMisc(Token& tok, char c);

        static bool             isSymbolChar(char c);
    };
}

#endif
