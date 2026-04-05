/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "interfaceBase.hpp"
#include "../interfaceLinksRegistry.hpp"
#include <dci/idl/contract/lidRegistry.hpp>
#include <dci/idl/introspection.hpp>

namespace dci::stiac::link
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <idl::interface::Side> class C, idl::interface::Side s>
    InterfaceBase<C, s>::InterfaceBase(const C<s>& from)
        : C<s>(from)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <idl::interface::Side> class C, idl::interface::Side s>
    InterfaceBase<C, s>::InterfaceBase(C<s>&& from)
        : C<s>(std::move(from))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <idl::interface::Side> class C, idl::interface::Side s>
    InterfaceBase<C, s>::InterfaceBase(const idl::Interface& from)
        : C<s>(from)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <idl::interface::Side> class C, idl::interface::Side s>
    InterfaceBase<C, s>::InterfaceBase(idl::Interface&& from)
        : C<s>(std::move(from))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <idl::interface::Side> class C, idl::interface::Side s>
    InterfaceBase<C, s>::~InterfaceBase()
    {
        (void)_registratorStub;
        _sbsOwner.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <idl::interface::Side> class C, idl::interface::Side s>
    void InterfaceBase<C, s>::initialize(Hub4Link* hub, Id id)
    {
        Base::initialize(hub, id);

        this->involvedChanged() += _sbsOwner * [this](bool v)
        {
            if(!v)
            {
                _sbsOwner.flush();
                if(_hub)
                {
                    std::exchange(_hub, nullptr)->linkUninvolved(_id, Hub4Link::uf_beginRemove | Hub4Link::uf_sendBegin);
                }
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <idl::interface::Side> class C, idl::interface::Side s>
    void InterfaceBase<C, s>::deinitialize()
    {
        Base::deinitialize();
        _sbsOwner.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <idl::interface::Side> class C, idl::interface::Side s>
    BasePtr InterfaceBase<C, s>::factory1(idl::Interface&& interface)
    {
        dbgAssert(interface);
        return BasePtr(new  Impl<C<s>>{std::move(interface)});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <idl::interface::Side> class C, idl::interface::Side s>
    BasePtr InterfaceBase<C, s>::factory2(idl::Interface& interface)
    {
        dbgAssert(!interface);
        C<s> i;
        i.init();
        interface = i.opposite();
        return BasePtr(new  Impl<C<s>>{std::move(i)});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <idl::interface::Side> class C, idl::interface::Side s>
    volatile const bool InterfaceBase<C, s>::_registratorStub = interfaceLinksRegistry.registrate(
                idl::introspection::typeName<C<s>>.data(),
                idl::interface::Lid{idl::contract::lidRegistry.emplace(idl::contract::MdDescriptor<C>::_id), s},
                &InterfaceBase<C, s>::factory1,
                &InterfaceBase<C, s>::factory2);

}
