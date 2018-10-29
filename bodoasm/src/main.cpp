
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
        out.emplace_back( bodoasm::StringPool::toStr(i) );

    return out;
}

#include "error.h"
#include "assembler.h"

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
            Assembler maker( getPathToLua(), "6502" );
            maker.doFile("test.txt");
            
            char c;
            std::cin >> c;
            
            return 0;
        }
        catch(bodoasm::Error&) {}
        catch(bodoasm::FatalError&) {}
        catch(std::exception& e)
        {
            std::cerr << "Unknown Error:  " << e.what() << std::endl;
        }
    }

    char c;
    std::cin >> c;
    return 1;
}
