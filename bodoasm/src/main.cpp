
#include <dshfs.h>
#include <luawrap.h>
#include <iostream>
#include "assembler.h"
#include "asmdefinition.h"
#include "stringpool.h"

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

std::vector<std::string> getNames(const std::vector<unsigned>& vec)
{
    std::vector<std::string> out;
    out.reserve(vec.size());

    for(auto& i : vec)
        out.emplace_back( bodoasm::StringPool::get(i) );

    return out;
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
            AsmDefinition face(err);
            face.load("E:/Projects/bodobon/bodoasm/lua/6502.lua","6502.lua");

            Position p{0,0,0};
            std::string s = "LDA";
            auto lda = getNames( face.getAddrModeForMnemonic(p, s) );
            s = "LDA.w";
            auto ldaw = getNames( face.getAddrModeForMnemonic(p, s) );
            s = "BPL.w";
            auto bplw = getNames( face.getAddrModeForMnemonic(p, s) );

            int butt = 5;

        }
        catch(bodoasm::Error&) {}
        catch(bodoasm::FatalError&) {}
        catch(std::exception& e)
        {
            std::cerr << "Error:  " << e.what() << std::endl;
        }
    }

    char c;
    std::cin >> c;
}
