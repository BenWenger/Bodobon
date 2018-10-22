#ifndef BODOASM_SRCFILEMANAGER_H_INCLUDED
#define BODOASM_SRCFILEMANAGER_H_INCLUDED

#include <string>
#include <vector>
#include <dshfs.h>

namespace bodoasm
{
    class SrcFileManager
    {
    public:
        struct Block
        {
            std::string     line;
            std::string     fileName;
            unsigned        lineNo;
        };

        bool                next(Block& blk);
        void                runNewFile(const std::string& filename);

    private:
        struct InternalBlock
        {
            InternalBlock(const std::string filename)
                : file(filename)
            {}
            Block               blk;
            dshfs::FileStream   file;
        };

        std::vector<InternalBlock>      blocks;

        static const std::size_t        maxBlockDepth = 100;    // pretty arbitrary
    };

}


#endif