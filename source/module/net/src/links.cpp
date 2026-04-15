/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
            if(_interfacesInitial.charged())
                return _interfacesInitial.future();
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
        _interfacesInitial.uncharge();
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
    void Links::flushChanges(bool complete)
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

        if(complete && _interfacesInitial.charged())
        {
            _interfacesInitial.resolveValue(_interfaces);
            _interfacesInitial.uncharge();
        }

        for(auto& p : added)
        {
            (*_iface)->linkAdded(p.first, _interfaces[p.first]);
        }
    }
}
