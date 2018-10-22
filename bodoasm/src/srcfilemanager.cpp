
#include <stdexcept>
#include "srcfilemanager.h"

namespace bodoasm
{
    void SrcFileManager::runNewFile(const std::string& filename)
    {
        InternalBlock blk(filename);
        blk.blk.fileName =      filename;
        blk.blk.lineNo =        0;

        if(!blk.file.good())
            throw std::runtime_error("Unable to open file: '" + filename + "' for reading.");

        blocks.emplace_back( std::move(blk) );
    }

    bool SrcFileManager::next(Block& out)
    {
        bool good = false;
        while(!good && !blocks.empty())
        {
        }
    }

}


/*
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
            Block               blk;
            dshfs::File::Ptr    file;
        };

        std::vector<InternalBlock>      blocks;
    };

}


#endif
*/