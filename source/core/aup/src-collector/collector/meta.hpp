/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include "absAndRel.hpp"

namespace dci::aup::collector
{
    struct Meta
    {
        Meta(Meta* parent, const std::string& name);
        virtual ~Meta() = default;

        virtual bool setup(const std::string& key, const std::vector<std::string>& values);

    protected:
        static bool emplacePairs(const std::vector<std::string>& values, auto& dst);

        static bool checkIsAbsDir(const std::filesystem::path& path);
        static bool checkIsAbsFile(const std::filesystem::path& path);
        static bool checkIsAbsDir(const std::string& path);
        static bool checkIsAbsFile(const std::string& path);

        static bool checkFirstIsAbsDirs(const auto& pairs);
        static bool checkFirstIsAbsFiles(const auto& pairs);
        static bool checkIsAbsFiles(const auto& paths);

    public:
        Meta* _parent;

    public:
        std::string                         _name;

        std::set<AbsAndRel>                 _dirMapping;

        std::set<AbsAndRel>                 _resourceDirs;
        std::set<AbsAndRel>                 _resourceFiles;

        std::map<std::string, std::string>  _syslibMapTo;
        std::set<std::string>               _syslibIgnore;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Meta::emplacePairs(const std::vector<std::string>& values, auto& dst)
    {
        for(std::size_t i{}; i<values.size(); i+=2)
        {
            std::string key = values[i];
            std::string val = values.size() > i+1 ? values[i+1] : std::string{};

            dst.emplace(
                        fs::path{key}.lexically_normal().string(),
                        fs::path{val}.lexically_normal().string());
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Meta::checkFirstIsAbsDirs(const auto& pairs)
    {
        for(const auto& pair : pairs)
        {
            if(!checkIsAbsDir(std::get<0>(pair)))
            {
                return false;
            }
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Meta::checkFirstIsAbsFiles(const auto& pairs)
    {
        for(const auto& pair : pairs)
        {
            if(!checkIsAbsFile(std::get<0>(pair)))
            {
                return false;
            }
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Meta::checkIsAbsFiles(const auto& paths)
    {
        for(const auto& path : paths)
        {
            if(!checkIsAbsFile(path))
            {
                return false;
            }
        }

        return true;
    }

}
