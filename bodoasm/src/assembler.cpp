
#include <iomanip>
#include "assembler.h"

namespace bodoasm
{

    Assembler::Assembler(const std::string pathToLua, const std::string pathToSrc)
    {
        //luaL_openlibs(lua);
        
        srcFileMgr.runNewFile("tester.txt");
        while(auto* i = srcFileMgr.next())
        {
            std::cout << std::setw(10) << i->fileName << std::setw(3) << i->lineNo << ":  " << i->line << std::endl;
            if(i->lineNo == 5)
                srcFileMgr.runNewFile("butt.txt");
        }
    }

}
