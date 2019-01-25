
#ifndef BODOASM_COMMANDLINEARGS_H_INCLUDED
#define BODOASM_COMMANDLINEARGS_H_INCLUDED

#include <string>
#include <vector>
#include "types/basetypes.h"

/*
    -d <dir>        Object file directory.  When creating object files, they will be placed in this dir, 
                        when linking, they will be taken from this directory
    -a <mode>       Assemble.  The arg is the mode to assemble with (ie:  6502/spc700/65816)
    -o <outfile>    Output binary.  outfile is the binary file name
    -f              Full overwrite (output file is erased before output).  Must be used with -o
    -w              Treat warnings as errors (no output if warning)
    -D <dir>        Lua directory for asm defs.  If the lua does not exist here, it will look in the
                        default directory
                        */

namespace bodoasm
{
    struct CommandLineArgs
    {
        std::vector<std::string>    inFileNames;
        std::string                 outFileName;
        std::string                 altLuaDirectory;
        std::string                 asmMode;
        bool                        warningAsErrors     = false;
        bool                        fullOverwrite       = false;

        static CommandLineArgs      parseCommandLine(int argc, const char* argv[]);
    };
}

#endif