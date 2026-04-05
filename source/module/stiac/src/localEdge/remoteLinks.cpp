/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "remoteLinks.hpp"

namespace dci::module::stiac::localEdge
{
    namespace
    {
//        link::RemoteId idx2Id(uint64 idx)
//        {
//            return static_cast<link::RemoteId>(idx+1);
//        }

        uint64 id2Idx(link::RemoteId id)
        {
            dbgAssert(link::RemoteId::null != id);
            return static_cast<uint64>(id)-1;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    RemoteLinks::RemoteLinks()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    RemoteLinks::~RemoteLinks()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool RemoteLinks::emplace(link::BasePtr&& link, link::RemoteId id)
    {
        uint64 idx = id2Idx(id);

        if(_zombieList.contains(idx))
        {
            return false;
        }

        if(_links.size() <= idx)
        {
            _links.resize(idx+1);
            _links[idx] = std::move(link);
            return true;
        }
        else if(!_links[idx])
        {
            _links[idx] = std::move(link);
            return true;
        }

        //ячейка уже занята
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    link::Base* RemoteLinks::get(link::RemoteId id)
    {
        uint64 idx = id2Idx(id);

        if(idx >= _links.size())
        {
            return nullptr;
        }

        return _links[idx].get();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool RemoteLinks::remove(link::RemoteId id)
    {
        uint64 idx = id2Idx(id);

        if(_zombieList.contains(idx))
        {
            return false;
        }

        if(idx >= _links.size())
        {
            return false;
        }

        if(!_links[idx])
        {
            return false;
        }

        _links[idx].reset();

        while(!_links.empty() && !_links.back())
        {
            _links.pop_back();
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool RemoteLinks::beginRemove(link::RemoteId id)
    {
        uint64 idx = id2Idx(id);

        if(idx >= _links.size())
        {
            return false;
        }

        if(!_links[idx])
        {
            return false;
        }

        return _zombieList.insert(idx).second;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool RemoteLinks::endRemove(link::RemoteId id)
    {
        uint64 idx = id2Idx(id);

        auto iter = _zombieList.find(idx);

        if(_zombieList.end() == iter)
        {
            return false;
        }

        if(idx >= _links.size())
        {
            return false;
        }

        if(!_links[idx])
        {
            return false;
        }

        _zombieList.erase(iter);
        _links[idx].reset();

        while(!_links.empty() && !_links.back())
        {
            _links.pop_back();
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void RemoteLinks::deinitialize()
    {
        for(const link::BasePtr& link : _links)
        {
            if(link)
            {
                link->deinitialize();
            }
        }
    }
}
