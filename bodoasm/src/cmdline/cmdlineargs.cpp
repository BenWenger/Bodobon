
#include "cmdlineargs.h"
#include <set>
#include <deque>
#include <stdexcept>

namespace bodoasm
{
    CommandLineArgs CommandLineArgs::parseCommandLine(int argc, const char* argv[])
    {
        std::string         v;
        std::set<char>      givenArgs;
        std::deque<char>    queue;
        CommandLineArgs     out;

        for(int i = 1; i < argc; ++i)
        {
            v = argv[i];
            if(v.empty())
                continue;       // this shouldn't happen, but whatever

            if(v[0] == '-')     // option?
            {
                for(std::size_t x = 1; x < v.size(); ++x)
                {
                    char opt = v[x];
                    auto didInsert = givenArgs.insert(opt);
                    if(!didInsert.second)
                        throw std::runtime_error(std::string("Option -") + opt + " cannot be given multiple times");

                    switch(opt)
                    {
                    case 'a': case 'o': case 'D':
                        queue.push_back(opt);
                        break;
                    case 'f':
                        out.fullOverwrite = true;
                        break;
                    case 'w':
                        out.warningAsErrors = true;
                        break;
                    default:
                        throw std::runtime_error(std::string("Unknown Option: -") + opt);
                    }
                }
            }
        }
    }
}