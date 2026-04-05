/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
