/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/idl/interface.hpp>
#include <dci/idl/interface/id.hpp>
#include <dci/idl/contract/lidRegistry.hpp>
#include "../../interfaceLinksRegistry.hpp"
#include "../base.hpp"

namespace dci::stiac::link::serialization
{
    struct NonMdInterface
    {
        idl::Interface      _instance;
        idl::interface::Lid _targetLid = {};
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void save(auto& ar, NonMdInterface&& v)
    {
        if(v._instance)
        {
            idl::interface::Lid lid = v._targetLid ? v._targetLid : v._instance.mdLid();

            BasePtr link = interfaceLinksRegistry.create1(lid, std::move(v._instance));
            if(!link)
            {
                std::string details = "target interface stiac link unavailable: " + idl::contract::lidRegistry.get(lid._clid).toHex();
                ar.fail(details.c_str());
                ar << LocalId::null;
            }
            else
            {
                LocalId localId = ar.emplaceLink(std::move(link));
                ar << localId << lid;
            }
        }
        else
        {
            ar << LocalId::null;
        }
    }
}

namespace dci::stiac::link::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void save(auto& ar, const idl::Interface& v)
    {
        save(ar, NonMdInterface{v, v ? v.mdLid() : idl::interface::Lid{}});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void save(auto& ar, idl::Interface&& v)
    {
        save(ar, NonMdInterface{std::move(v), v ? v.mdLid() : idl::interface::Lid{}});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void load(auto& ar, idl::Interface& v)
    {
        RemoteId remoteId;
        ar >> remoteId;

        if(RemoteId::null != remoteId)
        {
            idl::interface::Lid lid;
            ar >> lid;
            lid._side = invert(lid._side);

            BasePtr link = interfaceLinksRegistry.create2(lid, v);
            if(!link)
            {
                std::string details = "target interface stiac link unavailable: " + idl::contract::lidRegistry.get(lid._clid).toHex();
                ar.fail(details.c_str());
                return;
            }

            if(!ar.emplaceLink(std::move(link), remoteId))
            {
                ar.fail("malformed input");
            }
        }
        else
        {
            v = idl::Interface();
        }
    }
}

namespace dci::stiac::link::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <idl::interface::Side> class C, idl::interface::Side s>
    void save(auto& ar, const C<s>& v) requires(idl::contract::MdDescriptor<C>::_declared && !Impl<C<s>>::_declared)
    {
        if(v)
        {
            save(ar, NonMdInterface{idl::Interface{v}, v.lid()});
        }
        else
        {
            save(ar, NonMdInterface{idl::Interface{}});
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <idl::interface::Side> class C, idl::interface::Side s>
    void save(auto& ar, C<s>&& v) requires(idl::contract::MdDescriptor<C>::_declared && !Impl<C<s>>::_declared)
    {
        if(v)
        {
            save(ar, NonMdInterface{idl::Interface{v}, v.lid()});
            v = C<s>();
        }
        else
        {
            save(ar, NonMdInterface{idl::Interface{}});
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <idl::interface::Side> class C, idl::interface::Side s>
    void load(auto& ar, C<s>& v) requires(idl::contract::MdDescriptor<C>::_declared && !Impl<C<s>>::_declared)
    {
        idl::Interface i;
        load(ar, i);
        v = i;
    }
}

namespace dci::stiac::link::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <idl::interface::Side> class C, idl::interface::Side s>
    void save(auto& ar, const C<s>& v) requires(idl::contract::MdDescriptor<C>::_declared && Impl<C<s>>::_declared)
    {
        LocalId localId = v ? ar.emplaceLink(BasePtr(new  Impl<C<s>>{v})) : LocalId::null;
        ar << localId;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <idl::interface::Side> class C, idl::interface::Side s>
    void save(auto& ar, C<s>&& v) requires(idl::contract::MdDescriptor<C>::_declared && Impl<C<s>>::_declared)
    {
        LocalId localId =  v ? ar.emplaceLink(BasePtr(new  Impl<C<s>>{std::move(v)})) : LocalId::null;
        ar << localId;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <idl::interface::Side> class C, idl::interface::Side s>
    void load(auto& ar, C<s>& v) requires(idl::contract::MdDescriptor<C>::_declared && Impl<C<s>>::_declared)
    {
        RemoteId remoteId;
        ar >> remoteId;

        if(RemoteId::null != remoteId)
        {
            if(!v)
            {
                v.init();
            }

            (void)ar.emplaceLink(BasePtr(new  Impl<C<invert(s)>>{static_cast<const C<invert(s)>&>(v)}), remoteId);
        }
        else
        {
            if(v)
            {
                v = C<s>();
            }
        }
    }
}
