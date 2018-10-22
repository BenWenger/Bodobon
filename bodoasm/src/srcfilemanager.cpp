
#include <stdexcept>
#include "srcfilemanager.h"

namespace bodoasm
{
    void SrcFileManager::runNewFile(const std::string& filename)
    {
        if(blocks.size() >= maxBlockDepth)
            throw std::runtime_error("Maximum include depth exceeded when attempting to open file '" + filename + "'");

        auto ptr = std::make_unique<InternalBlock>(filename);
        if(!ptr->file.good())
            throw std::runtime_error("Unable to open file '" + filename + "' for reading");

        ptr->blk.fileName = filename;
        ptr->blk.lineNo = 0;
        blocks.emplace_back( std::move(ptr) );
    }

    const SrcFileManager::Block* SrcFileManager::next()
    {
        while(!blocks.empty())
        {
            if(blocks.back()->file.eof())
                blocks.pop_back();
            else
                break;
        }
        if(blocks.empty())
            return nullptr;

        auto& iblk = *blocks.back();
        auto& f = iblk.file;
        std::getline(f, iblk.blk.line);
            
        if(f.fail() && !f.eof())
            throw std::runtime_error("Unknown error occurred when reading file '" + iblk.blk.fileName + "'");

        iblk.blk.lineNo++;
        return &iblk.blk;
    }

}
