
#include <iostream>
#include <stdexcept>
#include <luawrap.h>
#include "json.h"
#include "data.h"

using namespace bodobeep;

int main()
{
    try
    {
        Data dat;

        auto song = dat.loadSong("../lua/17_song.bbs.json");

        return 0;
    }catch(std::exception& e){
        std::cout << e.what() << std::endl;
    }catch(...){
        std::cout << "unknown error!" << std::endl;
    }

    return 1;
}