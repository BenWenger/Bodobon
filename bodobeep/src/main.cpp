
#include <iostream>
#include <stdexcept>
#include <luawrap.h>
#include "json.h"
#include "data.h"
#include "driver.h"

using namespace bodobeep;

int main()
{
    try
    {
        Data dat;

        auto song = dat.loadSong("../lua/05_song.bbs.json");

        song->host->driver->playSong(song);

        char c;
        std::cin >> c;

        return 0;
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
    }catch(...){
        std::cout << "unknown error!" << std::endl;
    }

    return 1;
}