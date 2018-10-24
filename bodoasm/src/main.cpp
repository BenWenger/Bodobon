
#include <dshfs.h>
#include <luawrap.h>
#include <iostream>
#include "assembler.h"

using luawrap::Lua;
using namespace dshfs;

int func(Lua& lua)
{
    std::cout << "In function!\n";
    return 0;
}

std::string getPathToLua()
{
    auto path = Filename(dshfs::fs.getExecutablePath()).getPathPart();
#ifdef _DEBUG
    path.append("../lua/");
#else
    path.append("lua/");
#endif
    return path;
}

#include "error.h"
#include "lexer.h"

int main(int argc, char* argv[])
{
    /*if(argc < 3)
    {
        std::cout << "Usage:  bodoasm <asm_mode> <file_name>\n\tSee readme for details." << std::endl;
    }
    else*/
    {
        try
        {
            using namespace bodoasm;
            ErrorReporter err;
            Lexer lex(err);
            lex.startFile("lextest.txt");
            Lexer::Token tok;
            do
            {
                tok = lex.getNext();
                auto strsiz = tok.str.length();
                int foo = 5;
            } while(tok.type != Lexer::Token::Type::InputEnd);
        }
        catch(std::exception& e)
        {
            std::cerr << "Error:  " << e.what() << std::endl;
        }
    }

    char c;
    std::cin >> c;
}
