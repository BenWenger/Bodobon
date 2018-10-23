#ifndef BODOASM_SRCFILEMANAGER_H_INCLUDED
#define BODOASM_SRCFILEMANAGER_H_INCLUDED

#include <string>
#include <vector>
#include <memory>
#include <dshfs.h>

#include "position.h"

namespace bodoasm
{
    class ErrorReporter;

    class SrcFileManager
    {
    public:
        typedef std::size_t         fileid_t;

                            SrcFileManager(ErrorReporter& er);
        bool                next(Position& pos, std::string& str);
        void                runNewFile(const std::string& filename, Position* errposition = nullptr);

    private:
        struct Block
        {
            Block(const std::string filename)
                : file(filename)
            {}
            Position            pos;
            dshfs::FileStream   file;
        };
        static const std::size_t        maxBlockDepth = 1000;       // pretty arbitrary


        std::vector<std::unique_ptr<Block>> blocks;
        ErrorReporter&                      err;
        bool                                eofReached;
    };

}


#endif