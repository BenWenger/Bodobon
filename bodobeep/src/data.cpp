
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
        fn.fullResolve();
        return fn.getFullPath();
    }

    //////////////////////////////////////////////////////////////////////
    //  Song loading

    std::unique_ptr<Song> Data::loadSong(const std::string& filename)
    {
        try
        {
            std::string path = dshfs::fs.makeAbsolute(filename);

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

            return std::move(song);
        }
        catch(std::exception& e)
        {
            throw std::runtime_error("When loading song (" + filename + "):  " + e.what());
        }
        return nullptr;
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
        if(!chani->second.is<json::object>())       throw std::runtime_error("Song's '_channels' tag is not an object");
        auto& chanobj = chani->second.get<json::object>();

        for(auto& i : chanobj)
            loadChannel(i.first, i.second);

        // drop _channels
        file.obj.erase(chani);

        // the rest is userData
        userData = std::move(file.obj);

        // Now that everything is loaded, register us with the driver
        host->driver->registerSong(this);

        // Now that it's registered, recalcuate the tone chain
        for(auto& i : channels)
            recalculateToneChain(i);
    }

    void Song::loadChannel(const std::string& chanName, json::value& chanv)
    {
        // TODO verify that 'chanName' is a valid channel name (ask the Driver)

        Channel chanObj;
        chanObj.name = chanName;

        if(!chanv.is<json::object>())               throw std::runtime_error("One or more entries in the '_channels' entry is not an object");
        auto& ch = chanv.get<json::object>();
        
        // Load '_proprties' tag
        auto propi = ch.find("_properties");
        if(propi == ch.end())                       throw std::runtime_error("Channel is missing '_properties' tag");
        if(!propi->second.is<json::object>())       throw std::runtime_error("Channel's '_properties' tag is not an object");
        auto& prop = propi->second.get<json::object>();

        {
            auto i = prop.find("_loopPos");
            if(i != prop.end())
            {
                if(!i->second.is<std::int64_t>())
                    throw std::runtime_error("Channel's _loopPos property is not an integer");
                auto loopPos = i->second.get<std::int64_t>();
                if(loopPos < 0 || loopPos >= std::numeric_limits<timestamp_t>::max())
                    throw std::runtime_error("Channel's _loopPos property is out of range");

                chanObj.loopPos = static_cast<timestamp_t>( loopPos );
            }
        }

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
            //  We can't build the tone chain PROPERLY here, because we'd have to call the driver
            //    to get the tone length, which we can't do yet because this song has not yet been
            //    registered in the driver (because it hasn't been loaded yet)
            //
            //  So just give all tones a length of 1, and then correct it later
            Tone t;
            t.userData = std::move(i);
            t.length = 1;
            chanObj.score.addTone( position, std::move(t) );
            ++position;
        }

        if(chanObj.loopPos >= 0 && chanObj.loopPos >= chanObj.score.size())
            throw std::runtime_error("Channel's _loopPos property is out of range");

        // now that the chanObj is loaded, put it in our array
        channels.emplace_back( std::move(chanObj) );
    }

    void Song::recalculateToneChain(Channel& chan)
    {
        ToneChain out;

        timestamp_t pos = 0;
        for(auto& i : chan.score)
        {
            auto& tone = i.second;
            auto length = host->driver->getLengthOfTone(tone, chan.name, this);

            tone.length = length;
            out.addTone(pos, std::move(tone));
            pos += length;
        }

        chan.score = std::move(out);
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
