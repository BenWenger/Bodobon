
#include <dshfs.h>
#include "data.h"
#include "driver.h"
#include "jsonfile.h"
#include "json.h"

namespace bodobeep
{
    std::string Data::makeAbsoluteRelativeTo(const std::string& srcPath, const std::string& dstFile)
    {
        // make the host name absolute, but relative to the srcPath directory, rather 
        //   than the CWD
        if(dshfs::fs.isAbsolute(dstFile))
            return dstFile;

        dshfs::Filename fn(srcPath);
        fn.setFullPath( fn.getPathPart() + dstFile );
        fn.makeAbsolute();
        return fn.getFullPath();
    }

    //////////////////////////////////////////////////////////////////////
    //  Song loading

    Song* Data::loadSong(const std::string& filename)
    {
        Song* out = nullptr;
        try
        {
            std::string path = dshfs::fs.makeAbsolute(filename);

            // do we already have this song loaded?
            {
                auto i = songs.find(path);
                if(i != songs.end())
                    return i->second.get();
            }

            // parse the json file
            JsonFile file;
            {
                dshfs::FileStream stream(path);
                file.load(stream);
            }

            if(file.fileType != JsonFile::FileType::Song)
                throw std::runtime_error("Not a song file");

            // Load the first half of the song
            auto song = std::make_unique<Song>();
            song->loadFirstHalf(file);

            // Use that to get the host (and driver)
            song->hostPath  = makeAbsoluteRelativeTo(path, song->hostPath);
            song->host      = loadHost(song->hostPath);

            // Then we can load the rest
            song->loadSecondHalf(file);

            out = song.get();
            songs[path] = std::move(song);
        }
        catch(std::exception& e)
        {
            throw std::runtime_error("When loading song (" + filename + "):  " + e.what());
        }
        return out;
    }

    void Song::loadFirstHalf(JsonFile& file)
    {
        // Load '_proprties' tag
        auto propi = file.obj.find("_properties");
        if(propi == file.obj.end())                 throw std::runtime_error("Song is missing '_properties' tag");
        if(!propi->second.is<json::object>())       throw std::runtime_error("Song's '_properties' tag is not an object");
        auto& prop = propi->second.get<json::object>();

        // Load '_host' property
        auto i = prop.find("_host");
        if(i == prop.end())                         throw std::runtime_error("Song is missing '_host' property");
        if(!i->second.is<std::string>())            throw std::runtime_error("Song's '_host' property is not a string");
        hostPath = i->second.get<std::string>();

        // Remove the _properties from the json file -- we're done with it
        file.obj.erase(propi);
    }
    
    void Song::loadSecondHalf(JsonFile& file)
    {
        // Second half is the channel and score data!
        auto chani = file.obj.find("_channels");
        if(chani == file.obj.end())                 throw std::runtime_error("Song is missing '_channels' tag");
        if(!chani->second.is<json::object>())        throw std::runtime_error("Song's '_channels' tag is not an object");
        auto& chanobj = chani->second.get<json::object>();

        for(auto& i : chanobj)
            loadChannel(i.first, i.second);

        // drop _channels
        file.obj.erase(chani);

        // the rest is userData
        userData = std::move(file.obj);
    }

    void Song::loadChannel(const std::string& chanName, json::value& chanv)
    {
        // TODO verify that 'chanName' is a valid channel name (ask the Driver)

        Channel chanObj;

        if(!chanv.is<json::object>())               throw std::runtime_error("One or more entries in the '_channels' entry is not an object");
        auto& ch = chanv.get<json::object>();
        
        // Load '_proprties' tag
        auto propi = ch.find("_properties");
        if(propi == ch.end())                       throw std::runtime_error("Channel is missing '_properties' tag");
        if(!propi->second.is<json::object>())       throw std::runtime_error("Channel's '_properties' tag is not an object");
        auto& prop = propi->second.get<json::object>();

        // TODO - loopPos property

        // Drop all other properties
        ch.erase(propi);

        // Load the score!!!!
        auto scorei = ch.find("_score");
        if(scorei == ch.end())                      throw std::runtime_error("Channel is missing '_score' tag");
        if(!scorei->second.is<json::array>())       throw std::runtime_error("Channel's '_score' tag is not an array");
        auto& score = scorei->second.get<json::array>();

        timestamp_t position = 0;
        for(auto& i : score)
        {
            Tone t;
            t.userData = std::move(i);
            t.length = host->driver->getLengthOfTone(t, chanName, 0);       // TODO song index
            auto tmp = t.length;
            chanObj.score.addTone( position, std::move(t) );
            position += tmp;
        }

        // now that the chanObj is loaded, put it in our array
        channels.emplace_back( std::move(chanObj) );
    }
    
    //////////////////////////////////////////////////////////////////////
    //  Host loading

    Host* Data::loadHost(const std::string& path)
    {
        Host* out = nullptr;
        try
        {
            // do we already have this host loaded?
            {
                auto i = hosts.find(path);
                if(i != hosts.end())
                    return i->second.get();
            }

            // parse the json file
            JsonFile file;
            {
                dshfs::FileStream stream(path);
                file.load(stream);
            }

            if(file.fileType != JsonFile::FileType::Host)
                throw std::runtime_error("Not a host file");

            auto host = std::make_unique<Host>();
            host->load(file);

            // Use that to get the driver
            host->driverPath    = makeAbsoluteRelativeTo(path, host->driverPath);
            host->driver        = std::move(loadDriver(host.get(), host->driverPath));

            out = host.get();
            hosts[path] = std::move(host);
        }
        catch(std::exception& e)
        {
            throw std::runtime_error("When loading host file (" + path + "):  " + e.what());
        }
        return out;
    }

    void Host::load(JsonFile& file)
    {
        // Load '_proprties' tag
        auto propi = file.obj.find("_properties");
        if(propi == file.obj.end())                 throw std::runtime_error("Host is missing '_properties' tag");
        if(!propi->second.is<json::object>())       throw std::runtime_error("Host's '_properties' tag is not an object");
        auto& prop = propi->second.get<json::object>();

        // Load '_driver' property
        auto i = prop.find("_driver");
        if(i == prop.end())                         throw std::runtime_error("Host is missing '_driver' property");
        if(!i->second.is<std::string>())            throw std::runtime_error("Host's '_driver' property is not a string");
        driverPath = i->second.get<std::string>();

        // Remove the _properties from the json file -- we're done with it
        file.obj.erase(propi);

        // The rest is user data
        userData = std::move(file.obj);
    }

    Host::~Host()
    {
        // Put this in the cpp file because the Driver dtor isn't available in the header.
    }

    //////////////////////////////////////////////////////////////////////
    //  Driver loading

    std::unique_ptr<Driver> Data::loadDriver(const Host* host, const std::string& path)
    {
        auto driver = std::make_unique<Driver>(host, path);
        return std::move(driver);
    }
}
