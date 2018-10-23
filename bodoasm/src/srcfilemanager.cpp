
#include <stdexcept>
#include "srcfilemanager.h"
#include "filenamepool.h"
#include "error.h"

namespace bodoasm
{
    SrcFileManager::SrcFileManager(ErrorReporter& er)
        : err(er)
    {}

    void SrcFileManager::runNewFile(const std::string& filename, Position* errposition)
    {
        Position pos;
        pos.fileId = FilenamePool::add(filename);
        pos.lineNo = 0;
        pos.linePos = 0;

        if(blocks.size() >= maxBlockDepth)
            err.fatal(errposition, "Maximum include depth exceeded when attempting to open file '" + filename + "'");

        auto ptr = std::make_unique<Block>(filename);
        if(!ptr->file.good())
            err.error(errposition, "Unable to open file '" + filename + "' for reading");

        ptr->pos = pos;
        blocks.emplace_back( std::move(ptr) );
    }

    bool SrcFileManager::next(Position& pos, std::string& str)
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

        auto& blk = *blocks.back();
        auto& f = blk.file;
        std::getline(f, str);
            
        if(f.fail() && !f.eof())
            err.fatal(nullptr, "Unknown error occurred when reading file '" + FilenamePool::get(blk.pos.fileId) + "'");

        blk.pos.lineNo++;
        pos = blk.pos;
        return true;
    }

}
