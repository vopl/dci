/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "../collector.hpp"
#include <dci/utils/h2b.hpp>

namespace dci::aup
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Collector::setBuildDir(std::string v)
    {
        auto p = fs::canonical(v);
        if(!fs::is_directory(p))
        {
            throw std::runtime_error{"bad build directory path: "+v+" ("+p.string()+")"};
        }

        _buildDir = p;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Collector::setMetaFile(std::string v)
    {
        auto p = fs::canonical(v);
        if(!fs::is_regular_file(p))
        {
            throw std::runtime_error{"bad meta file path: "+v+" ("+p.string()+")"};
        }

        _metaFile = p;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Collector::setVendorKey(std::string v)
    {
        if(v.size() != _vendorKey.size()*2 ||
            !dci::utils::h2b(v.data(), v.size(), _vendorKey.data()))
        {
            throw std::runtime_error{"bad vendor key value: "+v};
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Collector::setStorageDir(std::string v)
    {
        fs::create_directories(v);
        auto p = fs::canonical(v);
        if(!fs::is_directory(p))
        {
            throw std::runtime_error{"bad resulting storage dir: "+v+" ("+p.string()+")"};
        }

        _storageDir = p;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Collector::setIgnoreSources(bool v)
    {
        _ignoreSources = v;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Collector::setIgnoreDebug4Targets(bool v)
    {
        _ignoreDebug4Targets = v;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Collector::setIgnoreDebug4Others(bool v)
    {
        _ignoreDebug4Others = v;
    }
}
