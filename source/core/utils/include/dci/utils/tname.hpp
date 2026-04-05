/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <array>
#include <algorithm>

namespace dci::utils
{
    namespace tnameDetails
    {
        template <class T>
        constexpr auto fsig()
        {
            //TODO std::source_location::function_name
            return std::to_array(__PRETTY_FUNCTION__);
        }

        template <class T>
        constexpr auto name()
        {
            constexpr auto probe = tnameDetails::fsig<void>();
            constexpr auto probeTarget = std::to_array("void");
            constexpr std::size_t prefix = static_cast<std::size_t>(std::search(probe.begin(), probe.end()-1, probeTarget.begin(), probeTarget.end()-1) - probe.begin());
            constexpr std::size_t postfix = (probe.size()-1) - prefix - (probeTarget.size()-1);

            constexpr auto victim = tnameDetails::fsig<T>();

            std::array<char, victim.size()-prefix-postfix> res{};
            std::copy_n(victim.data()+prefix, res.size()-1, res.data());

            return res;
        }
    }

    template <class T>
    static constexpr auto tname = tnameDetails::name<T>();
}
