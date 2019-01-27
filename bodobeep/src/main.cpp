
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

        auto song = dat.loadSong("../lua/0F_song.bbs.json");
        //auto song = dat.loadSong("../lua/test.json");
        
        std::cout << "Load OK" << std::endl;

        song->host->driver->playSong(song);
        
        std::cout << "Play OK" << std::endl;

        char c;
        std::cin >> c;

        song->host->driver->stop();
        
        //std::cin >> c;

        return 0;
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
    }catch(...){
        std::cout << "unknown error!" << std::endl;
    }
    
    char c;
    std::cin >> c;
    return 1;
}