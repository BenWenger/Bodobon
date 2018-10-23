
#include "lexer.h"

namespace bodoasm
{

    Lexer::Lexer(SrcFileManager* src)
    {
        srcFile = src;
    }

    auto Lexer::getNext() -> Token
    {

    }

    void Lexer::unget(const Token& t)
    {
    }

}

