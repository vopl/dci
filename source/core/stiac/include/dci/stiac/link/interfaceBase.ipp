// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
