/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "../contract/wiresBundle.hpp"
#include "../contract/hmDescriptorBase.hpp"
#include "../contract/hmDescriptor.hpp"
#include "../contract/mdState.hpp"
#include <dci/utils/dbg.hpp>

namespace dci::idl::interface
{
    template <template<Side> class, Side>
    class StateHolder
    {
        template <template <Side> class, Side>
        friend struct Methods;

        template <template <Side> class, Side>
        friend class ImplBase;

        template <bool>
        friend class Generic;

    private:
        template <template <Side> class B>
        contract::WiresBundle<B>& getWiresBundle() const;

    private:
        const contract::HmDescriptorBase*   _hmcDescriptor;// без инициализации
        contract::MdState*                  _mdcState;// без инициализации
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    template <template <Side> class B>
    contract::WiresBundle<B>& StateHolder<C, s>::getWiresBundle() const
    {
        dbgAssertX(_mdcState && _hmcDescriptor, "use uninitialized interface instance");
        return _mdcState->getWiresBundle<contract::WiresBundle<B>>(_hmcDescriptor->hmbd(contract::HmDescriptor<C, B>::_index)._bundleOffset);
    }
}
