
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

        const_iterator      getAtTime(timestamp_t t) const
        {
            auto i = theMap.upper_bound(t);
            if(i != begin()) --i;
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

    private:
        map_t               theMap;
    };

}

#endif