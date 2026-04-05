/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "../collector.hpp"
#include <fstream>
#include <regex>
#include <cassert>

namespace dci::aup
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Collector::loadMeta()
    {
        if(_metaFile.empty())
        {
            return;
        }

        std::ifstream in{_metaFile};
        if(!in)
        {
            throw std::system_error(errno, std::generic_category(), _metaFile.string());
        }

        std::string line;
        std::size_t lineNum{};
        while(std::getline(in, line))
        {
            ++lineNum;

            static const std::regex rex1{R"(^\s*(\w+)\[([^\]]*)\]\s*)"};
            static const std::regex rex2{R"([\s;]*([^\s;]+))"};

            auto iter = line.cbegin();
            auto end = line.cend();
            std::smatch sm;

            collector::Unit* unit = nullptr;
            collector::Target* target = nullptr;
            for(; std::regex_search(iter, end, sm, rex1); iter = sm[0].second)
            {
                std::vector<std::string> values;
                std::copy(
                            std::sregex_token_iterator{sm[2].first, sm[2].second, rex2, 1},
                            std::sregex_token_iterator{},
                            std::back_inserter(values));

                if("UNIT" == sm[1])
                {
                    if(values.size() != 1)
                    {
                        throw std::logic_error{"bad unit name in " + _metaFile.string() + ":" + std::to_string(lineNum) + ":" + std::to_string(iter-line.begin())};
                    }

                    unit = &_unitsMeta.emplace(std::piecewise_construct_t{},
                                               std::tuple{values[0]},
                                               std::tuple{&_globalMeta, values[0]}).first->second;
                }
                else if("TARGET" == sm[1])
                {
                    if(values.size() != 1)
                    {
                        throw std::logic_error{"bad target name in " + _metaFile.string() + ":" + std::to_string(lineNum) + ":" + std::to_string(iter-line.begin())};
                    }

                    if(!unit)
                    {
                        throw std::logic_error{"target for unknown unit in " + _metaFile.string() + ":" + std::to_string(lineNum) + ":" + std::to_string(iter-line.begin())};
                    }

                    target = unit->getTarget(values[0], true);
                }
                else
                {
                    collector::Meta* dst = target ? target :
                                                    unit ? unit :
                                                           &_globalMeta;

                    if(!dst->setup(sm[1], values))
                    {
                        throw std::logic_error{"unable to setup meta in " + _metaFile.string() + ":" + std::to_string(lineNum) + ":" + std::to_string(iter-line.begin())};
                    }
                }
            }

            if(iter != end)
            {
                throw std::logic_error{"bad metafile content in " + _metaFile.string() + ":" + std::to_string(lineNum) + ":" + std::to_string(iter-line.begin())};
            }
        }
    }
}
