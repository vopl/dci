/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "link.hpp"

namespace dci::module::net
{

    namespace
    {
        inline bool addressEqual(const api::link::Ip4Address& a, const api::link::Ip4Address& b)
        {
            return
                    a.address.octets == b.address.octets &&
                    a.netmask.octets == b.netmask.octets &&
                    a.broadcast.octets == b.broadcast.octets;
        }
        inline bool addressEqual(const api::link::Ip6Address& a, const api::link::Ip6Address& b)
        {
            return
                    a.address.octets == b.address.octets &&
                    a.prefixLength == b.prefixLength&&
                    a.scope == b.scope;
        }

        template <class A>
        struct AddressEqualPred
        {
            AddressEqualPred(const A& a)
                : _a(a)
            {
            }

            A _a;

            bool operator()(const A& b) const
            {
                return addressEqual(_a, b);
            }
        };
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Link::Link(uint32 id)
        : api::Link<>::Opposite{idl::interface::Initializer{}}
        , _id(id)
    {
        methods()->id() += this * [&]
        {
            return cmt::readyFuture(_id);
        };

        methods()->name() += this * [&]
        {
            return cmt::readyFuture(_name);
        };

        methods()->flags() += this * [&]
        {
            return cmt::readyFuture(_flags);
        };

        methods()->mtu() += this * [&]
        {
            return cmt::readyFuture(_mtu);
        };

        methods()->hwAddr() += this * [&]
        {
            return cmt::readyFuture(_hwAddress);
        };

        methods()->addr() += this * [&]
        {
            List<api::link::Address> res;

            for(const auto& addr : _ip4)
            {
                res.emplace_back(addr);
            }

            for(const auto& addr : _ip6)
            {
                res.emplace_back(addr);
            }

            return cmt::readyFuture(res);
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Link::~Link()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Link::setHwAddress(const List<uint8>& v)
    {
        if(_hwAddress != v)
        {
            _hwAddress = v;
            _changed = true;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Link::setName(const std::string& v)
    {
        if(_name != v)
        {
            _name = v;
            _changed = true;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Link::setMtu(uint32 v)
    {
        if(_mtu != v)
        {
            _mtu = v;
            _changed = true;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Link::setFlags(idl::gen::net::link::Flags v)
    {
        if(_flags != v)
        {
            _flags = v;
            _changed = true;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Link::setIp4(List<api::link::Ip4Address> addresses)
    {
        if(_ip4 != addresses)
        {
            _ip4 = addresses;
            _changed = true;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Link::addIp4(api::link::Ip4Address address)
    {
        auto iter = std::find_if(_ip4.begin(), _ip4.end(), AddressEqualPred(address));
        //dbgAssert(_ip4.end() == iter);

        if(_ip4.end() == iter)
        {
            _ip4.emplace_back(address);
            _changed = true;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Link::delIp4(api::link::Ip4Address address)
    {
        auto iter = std::find_if(_ip4.begin(), _ip4.end(), AddressEqualPred(address));
        //dbgAssert(_ip4.end() != iter);

        if(_ip4.end() != iter)
        {
            _ip4.erase(iter);
            _changed = true;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Link::setIp6(List<api::link::Ip6Address> addresses)
    {
        if(_ip6 != addresses)
        {
            _ip6 = addresses;
            _changed = true;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Link::addIp6(api::link::Ip6Address address)
    {
        auto iter = std::find_if(_ip6.begin(), _ip6.end(), AddressEqualPred(address));
        //dbgAssert(_ip6.end() == iter);

        if(_ip6.end() == iter)
        {
            _ip6.emplace_back(address);
            _changed = true;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Link::delIp6(api::link::Ip6Address address)
    {
        auto iter = std::find_if(_ip6.begin(), _ip6.end(), AddressEqualPred(address));
        //dbgAssert(_ip6.end() == iter);

        if(_ip6.end() != iter)
        {
            _ip6.erase(iter);
            _changed = true;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Link::flushChanges()
    {
        if(_changed)
        {
            _changed = false;
            methods()->changed();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Link::remove()
    {
        if(!isInvolved())
        {
            delete this;
            return;
        }

        involvedChanged() += this * [this](bool v)
        {
            if(!v)
            {
                delete this;
            }
        };

        methods()->removed();
    }
}
