
#ifndef BODOBEEP_DATA_H_INCLUDED
#define BODOBEEP_DATA_H_INCLUDED

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "tonechain.h"

namespace bodobeep
{
    class JsonFile;
    class Song;
    class Host;
    class Driver;

    /////////////////////////////////////////////////////////////
    //   Songs
    class Song
    {
    public:
        struct Channel
        {
            std::string     name;
            int             loopPos = -1;
            ToneChain       score;
            json::object    userData;
        };
        typedef std::vector<Channel>    chanGroup_t;

        chanGroup_t         channels;
        std::string         hostPath;
        Host*               host        = nullptr;
        json::object        userData;
        
        void                loadFirstHalf(JsonFile& file);
        void                loadSecondHalf(JsonFile& file);

    private:
        void                loadChannel(const std::string& name, json::value& chanv);
        void                recalculateToneChain(Channel& chan);
    };
    
    /////////////////////////////////////////////////////////////
    //   Hosts
    class Host
    {
    public:
                                    ~Host();
        std::string                 driverPath;
        std::unique_ptr<Driver>     driver;
        json::object                userData;
        
        void                        load(JsonFile& file);
    };

    ////////////////////////////////////////////////////////////
    //  Data group

    class Data
    {
    public:
        typedef std::unordered_map<std::string, std::unique_ptr<Host>>      hostMap_t;
        hostMap_t               hosts;

        std::unique_ptr<Song>   loadSong(const std::string& filename);

    private:
        static std::string      makeAbsoluteRelativeTo(const std::string& srcPath, const std::string& dstFile);
        Host*                   loadHost(const std::string& path);
        std::unique_ptr<Driver> loadDriver(const Host* host, const std::string& path);
    };
}

#endif
