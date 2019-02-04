
#ifndef BODOBEEP_TONECHAIN_H_INCLUDED
#define BODOBEEP_TONECHAIN_H_INCLUDED

#include "types.h"
#include <map>
#include <vector>

namespace bodobeep
{
    class ToneChain
    {
    private:
        typedef std::map<timestamp_t, Tone>     map_t;

    public:
        typedef map_t::iterator                 iterator;
        typedef map_t::const_iterator           const_iterator;

        
        iterator            begin()                 { return theMap.begin();        }
        iterator            end()                   { return theMap.end();          }
        const_iterator      begin() const           { return theMap.begin();        }
        const_iterator      end() const             { return theMap.end();          }

        timestamp_t         size() const            { return static_cast<timestamp_t>( theMap.size() ); }

        const_iterator      getAtTime(timestamp_t t) const
        {
            auto i = theMap.upper_bound(t);
            if(i != begin())
                --i;
            if(i != end())
            {
                if(t >= (i->first + i->second.length))
                    i = end();
            }
            return i;
        }

        const_iterator      getNth(timestamp_t index) const
        {
            auto i = theMap.begin();
            for(timestamp_t ctr = 0; ctr < index && i != theMap.end(); ++ctr)
                ++i;
            return i;
        }

        void                initialize(std::vector<Tone>& vals)
        {
            theMap.clear();

            timestamp_t time = 0;
            for(auto& i : vals)
            {
                auto next = time + i.length;
                theMap[time] = std::move(i);
                time = next;
            }
            vals.clear();
        }

        void                addTone(timestamp_t time, Tone&& tone)
        {
            theMap[time] = std::move(tone);
        }
        

        timestamp_t         getTotalLength()
        {
            if(theMap.empty())
                return 0;
            auto i = theMap.end();
            --i;
            return i->first + i->second.length;
        }

    private:
        map_t               theMap;
    };

}

#endif
