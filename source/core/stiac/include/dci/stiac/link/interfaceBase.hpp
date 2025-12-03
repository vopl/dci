// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/sbs/owner.hpp>
#include <dci/idl/interface.hpp>
#include "base.hpp"

namespace dci::stiac::link
{
    template <template <idl::interface::Side> class C, idl::interface::Side s>
    class InterfaceBase
        : public Base
        , protected C<s>
    {
    public:
        InterfaceBase(const C<s>& from);
        InterfaceBase(C<s>&& from);
        InterfaceBase(const idl::Interface& from);
        InterfaceBase(idl::Interface&& from);
        ~InterfaceBase() override;

        void initialize(Hub4Link* hub, Id id) override;
        void deinitialize() override;

    protected:
        sbs::Owner _sbsOwner;
        static BasePtr factory1(idl::Interface&& interface);
        static BasePtr factory2(idl::Interface& interface);
        static volatile const bool _registratorStub;
    };

}
