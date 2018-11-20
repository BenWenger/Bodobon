
#include <dshfs.h>
#include <luawrap.h>
#include <iostream>
#include "assembler/assembler.h"

std::string getPathToLua(const std::string& append)
{
    auto path = dshfs::Filename(dshfs::fs.getExecutablePath()).getPathPart();
    return path + append;
}

int main(int argc, char* argv[])
{
    if(argc < 4)
    {
        std::cout << "Usage:  bodoasm <asm_mode> <file_name> <out_filename> \n\tSee readme for details." << std::endl;
    }
    else
    {
        try
        {
            std::string modeStr = argv[1];
            std::string srcFile = argv[2];
            std::string dstFile = argv[3];
            std::string luapath = "lua/";
            if(argc > 4)
                luapath = argv[4];

            bodoasm::Assembler prog( getPathToLua(luapath), modeStr );
            prog.assembleFile( srcFile );
            if( prog.okToProceed() )        prog.finalize();
            if( prog.okToProceed() )
            {
                auto file = dshfs::fs.openFile( dstFile, dshfs::FileMode::rwb );
                prog.output(file);
            }

            bool ok = prog.okToProceed();
            std::cout << "Assembly " << (ok ? "completed successfully" : "failed") << ".  " << prog.getErrCount() << " Error(s), " << prog.getWarnCount() << " Warning(s)" << std::endl;

            return prog.okToProceed() ? 0 : 1;
        }
        catch(std::exception& e)
        {
            std::cerr << "Fatal Error:  " << e.what() << std::endl;
        }
        catch(...)
        {
            std::cerr << "Unknown fatal error" << std::endl;
        }
    }

    return 1;
}
