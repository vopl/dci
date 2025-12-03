// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
