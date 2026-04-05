/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "meta.hpp"
#include <iostream>
#include <cassert>

namespace dci::aup::collector
{
    Meta::Meta(Meta* parent, const std::string& name)
        : _parent{parent}
        , _name{name}
    {
    }

    bool Meta::setup(const std::string& key, const std::vector<std::string>& values)
    {
        if("DIR_MAPPING"    == key && emplacePairs(values, _dirMapping)     && checkFirstIsAbsDirs(_dirMapping)     ) return true;
        if("RESOURCE_DIR"   == key && emplacePairs(values, _resourceDirs)   && checkFirstIsAbsDirs(_resourceDirs)   ) return true;
        if("RESOURCE_FILE"  == key && emplacePairs(values, _resourceFiles)  && checkFirstIsAbsFiles(_resourceFiles) ) return true;
        if("SYSLIB_MAPTO"   == key && emplacePairs(values, _syslibMapTo)) return true;

        if("SYSLIB_IGNORE" == key)
        {
            for(const std::string& value : values)
            {
                _syslibIgnore.insert(fs::path{value}.lexically_normal().string());
            }
            return true;
        }

        std::cerr << "bad meta payload: " << key << "[";
        for(const std::string& v : values)
        {
            std::cerr << v << ";";
        }
        std::cerr << "]"<<std::endl;
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Meta::checkIsAbsDir(const std::filesystem::path& path)
    {
        if(!path.is_absolute() || !fs::is_directory(path))
        {
            std::cout << "not a normal absolute path to a directory: " << path << std::endl;
            return false;
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Meta::checkIsAbsFile(const std::filesystem::path& path)
    {
        if(!path.is_absolute() || !fs::is_regular_file(path))
        {
            std::cout << "not a normal absolute path to a file: " << path << std::endl;
            return false;
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Meta::checkIsAbsDir(const std::string& path)
    {
        return checkIsAbsDir(fs::path{path});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Meta::checkIsAbsFile(const std::string& path)
    {
        return checkIsAbsFile(fs::path{path});
    }
}
