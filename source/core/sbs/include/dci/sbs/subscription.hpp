/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/himpl.hpp>
#include <dci/sbs/implMetaInfo.hpp>
#include "api.hpp"
#include "owner.hpp"
#include <cstdint>

namespace dci::sbs
{
    class API_DCI_SBS Subscription
        : public himpl::FaceLayout<Subscription, impl::Subscription>
    {
        Subscription(const Subscription&) = delete;
        void operator=(const Subscription&) = delete;

    public:
        enum Flags
        {
            act4Next    = 0x01,
            act4Last    = 0x02,
            act         = act4Next | act4Last,

            del         = 0x10,
        };

    protected:
        using Activator = void (*)(Subscription*, void *, std::uint_fast8_t);

        Subscription(Activator activator, Owner* owner = nullptr);
        ~Subscription();

        void removeSelf();
    };
}
