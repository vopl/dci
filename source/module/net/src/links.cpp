// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "links.hpp"
#include "host.hpp"

namespace dci::module::net
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Links::Links(api::Host<>::Opposite* iface)
        : _iface(iface)
    {
        (*_iface)->links() += this * [&]
        {
            return cmt::readyFuture(_interfaces);
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Links::~Links()
    {
        flush();

        _deleted.clear();
        _added.clear();
        _implementations.clear();
        _interfaces.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Link* Links::getLink(uint32 id)
    {
        auto iter = _implementations.find(id);
        if(_implementations.end() != iter)
        {
            return iter->second.get();
        }

        iter = _added.find(id);
        if(_added.end() != iter)
        {
            return iter->second.get();
        }

        return nullptr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Link* Links::allocLink(uint32 id)
    {
        return new Link{id};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Links::allocatedLinkInitialized(uint32 id, Link* link)
    {
        dbgAssert(_implementations.end() == _implementations.find(id));
        dbgAssert(_added.end() == _added.find(id));

        _added[id].reset(link);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Links::delLink(uint32 id)
    {
        _deleted.insert(id);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Links::flushChanges()
    {
        Implementations added;
        added.swap(_added);

        Ids deleted;
        deleted.swap(_deleted);

        for(uint32 id : deleted)
        {
            auto iter = _implementations.find(id);

            if(_implementations.end() != iter)
            {
                Link* link = iter->second.release();

                _implementations.erase(iter);
                _interfaces.erase(_interfaces.find(id));

                link->flushChanges();
                link->remove();
            }
        }

        for(auto& p : added)
        {
            Link* link = p.second.get();
            _implementations[p.first] = std::move(p.second);
            _interfaces[p.first] = *link;
        }

        for(auto& p : _implementations)
        {
            p.second->flushChanges();
        }

        for(auto& p : added)
        {
            (*_iface)->linkAdded(p.first, _interfaces[p.first]);
        }
    }
}
