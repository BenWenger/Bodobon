
#include <dshfs.h>
#include <luawrap.h>
#include <iostream>
#include "assembler.h"

std::string getPathToLua()
{
    auto path = dshfs::Filename(dshfs::fs.getExecutablePath()).getPathPart();
#ifdef _DEBUG
    path.append("../lua/");
#else
    path.append("lua/");
#endif
    return path;
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

            bodoasm::Assembler prog( getPathToLua(), modeStr );
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
            std::cout << "Fatal Error:  " << e.what() << std::endl;
        }
        catch(...)
        {
            std::cout << "Unknown fatal error" << std::endl;
        }
    }

    return 1;
}
