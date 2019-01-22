
#ifndef BODOBEEP_EDITOR_H_INCLUDED
#define BODOBEEP_EDITOR_H_INCLUDED

#include <unordered_map>
#include <string>
#include <memory>

namespace bodobeep
{
    class Song;
    class Driver;

    class Editor
    {
    public:
        void            loadSong(const std::string& filename);
        void            play(const std::string& filename);
        void            stop();

    private:
        std::unordered_map<std::string, std::unique_ptr<Driver>>        driverList;
        std::unordered_map<std::string, std::unique_ptr<Song>>          songList;
    };
}

#endif
