
#include "tblfile.h"
#include "error/error.h"
#include "types/stringpool.h"
#include <dshfs.h>


//  TODO table files need to have some escape characters
//    ie:  \x00 for literal zero, \n, etc

namespace bodoasm
{
    namespace
    {
        int toDigit(char c)
        {
            if(c >= '0' && c <= '9')        return c-'0';
            if(c >= 'A' && c <= 'F')        return c-'A'+10;
            if(c >= 'a' && c <= 'f')        return c-'a'+10;
            return 0x100;
        }

        int toByte(const char* s)
        {
            return (toDigit(s[0]) << 4) | toDigit(s[1]);
        }
    }

    auto TblFile::load(ErrorReporter& err, const Position& pos, const std::string& path) -> Ptr
    {
        dshfs::FileStream       file(path);
        if(!file)               err.error(&pos, "Table file '" + path + "' could not be opened for reading");

        entryMap_t      entryMap;

        Position tblpos;
        tblpos.fileId = StringPool::toInt(path);
        std::string s;
        while(true)
        {
            s.clear();
            std::getline(file, s);
            ++tblpos.lineNo;

            if(s.empty())
            {
                if(!file)       break;
                else            continue;
            }

            Entry entry;
            bool ok = true;
            auto eq = s.find('=');
            if(eq == s.npos || eq == 0 || (eq % 2))
                ok = false;
            else
            {
                for(decltype(eq) i = 0; i < eq; i += 2)
                {
                    int x = toByte(s.data()+i);
                    if(x > 0xFF)
                    {
                        ok = false;
                        break;
                    }
                    entry.bin.push_back( static_cast<u8>(x) );
                }
            }

            if(ok)
            {
                entry.str = s.substr(eq+1);
                if(entry.str.empty())       ok = false;
            }


            // This entry is complete!
            if(ok)
                entryMap[entry.str[0]].emplace_back( std::move(entry) );
            else
                err.error(&pos, "Table file line is malformed '" + s + "' " + ErrorReporter::formatPosition(tblpos));
        }

        auto out = std::make_unique<TblFile>();
        out->entries = std::move(entryMap);
        return out;
    }

    void TblFile::toBinary(ErrorReporter& err, const Position& pos, std::vector<u8>& output, const std::string& str) const
    {
        std::size_t ctr = 0;
        while(ctr < str.size())
        {
            bool ok = false;
            auto i = entries.find(str[ctr]);
            if(i != entries.end())
            {
                for(auto& s : i->second)
                {
                    if(str.substr(ctr, s.str.size()) == s.str)
                    {
                        output.insert(output.end(), s.bin.begin(), s.bin.end());
                        ctr += s.str.size();
                        ok = true;
                        break;
                    }
                }
            }

            if(!ok)
            {
                char c = str[ctr];
                std::string code;
                if(c < ' ')     code = "{" + std::to_string(static_cast<int>(c)) + "}";
                else            code.push_back(c);

                err.error(&pos, "Table file has no entry for character '" + code + "'");
            }
        }
    }

}
