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
            std::string     fileName;       // TODO pool this
            unsigned        lineNo;
        };

        const Block*        next();
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

        std::vector<std::unique_ptr<InternalBlock>>     blocks;

        static const std::size_t        maxBlockDepth = 1000;       // pretty arbitrary
    };

}


#endif