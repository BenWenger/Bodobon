#ifndef BODOASM_LEXER_H_INCLUDED
#define BODOASM_LEXER_H_INCLUDED

#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include <dshfs.h>
#include "position.h"
#include "error.h"

namespace bodoasm
{
    class ErrorReporter;

    class Lexer
    {
    public:
        typedef std::int64_t            int_t;
        struct Token
        {
            enum class Type 
            {
                Operator,
                Symbol,
                Integer,
                String,
                CmdEnd,             // end of command (end of line or \ symbol)
                FileEnd,            // end of a single file
                InputEnd            // end of all input (file fully traversed)
            };
            Position                    pos;

            std::string                 str;                    // the operator/string/symbol
            int_t                       val = 0;                // the integer
            Type                        type = Type::InputEnd;
            bool                        ws_after = true;        // true if there is whitespace after this token
        };


                    Lexer(ErrorReporter& er);
        Token       getNext();
        void        unget(const Token& t);
        
        void        startFile(const std::string& filename);
        void        startInclude(const std::string& filename, const Position* errpos);


    private:
        typedef std::unique_ptr<dshfs::FileStream>  file_t;
        struct State
        {
            Position    pos;
            std::string text;
            file_t      file;

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
        std::vector<Token>      ungotten;
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
        void                    lexHexLiteral(Token& tok);
        void                    lexBinLiteral(Token& tok);
        void                    lexDecLiteral(Token& tok, char c);
        void                    lexSymbol(Token& tok, char c);

        static bool             isSymbolChar(char c);
    };
}

#endif
