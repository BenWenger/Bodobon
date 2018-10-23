#ifndef BODOASM_LEXER_H_INCLUDED
#define BODOASM_LEXER_H_INCLUDED

#include <string>
#include <cstdint>
#include <vector>
#include "srcfilemanager.h"

namespace bodoasm
{
    class Lexer
    {
    public:
        typedef std::int64_t            int_t;
        typedef SrcFileManager::Block   Block;
        struct Token
        {
            enum class Type 
            {
                Operator,
                Symbol,
                Integer,
                String,
                CmdEnd,             // end of command (end of line or \ symbol)
                InputEnd            // end of all input (all files fully traversed)
            };
            const Block*        blk;
            std::string         str;        // the operator/string/symbol
            int_t               val;        // the integer
            Type                type;
            bool                ws_after;   // true if there is whitespace after this token
        };


                    Lexer(SrcFileManager* src);
        Token       getNext();
        void        unget(const Token& t);


    private:
        SrcFileManager*         srcFile;
        std::vector<Token>      unGotten;

        SrcFileManager::Block   
    };
}

#endif
