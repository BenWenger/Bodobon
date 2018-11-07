
#ifndef BODOASM_TBLFILE_H_INCLUDED
#define BODOASM_TBLFILE_H_INCLUDED

#include <memory>
#include <string>
#include <vector>
#include <map>
#include "types/basetypes.h"
#include "types/position.h"

namespace bodoasm
{
    class ErrorReporter;

    class TblFile
    {
    public:
        typedef std::unique_ptr<TblFile>    Ptr;

        static Ptr      load(ErrorReporter& err, const Position& pos, const std::string& path);
        void            toBinary(ErrorReporter& err, const Position& pos, std::vector<u8>& output, const std::string& str) const;

    private:
        struct Entry
        {
            std::string         str;
            std::vector<u8>     bin;
        };
        typedef std::vector<Entry>          entryVec_t;
        typedef std::map<char, entryVec_t>  entryMap_t;
        entryMap_t                          entries;
    };
}

#endif
