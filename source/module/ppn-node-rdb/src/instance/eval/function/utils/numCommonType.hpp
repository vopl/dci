/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::node::rdb::instance::eval::function::utils::numCommonType
{
    decltype(auto) exec(const auto& valuesSeq, std::size_t maxCount, const auto& sink)
    {
        std::size_t count = std::min(valuesSeq.size(), maxCount);

        bool hasBool = false;
        bool hasUint = false;
        bool hasInt = false;
        bool hasReal = false;

        std::size_t minSize = std::numeric_limits<std::size_t>::max();
        std::size_t maxSize = 0;

        auto iter = valuesSeq.begin();
        for(std::size_t argIdx(0); argIdx<count; ++argIdx, ++iter)
        {
            iter->data.visit([&](const auto& v)
            {
                using T = std::decay_t<decltype(v)>;
                std::size_t size = 0;

                if constexpr(std::is_same_v<bool, T>)
                {
                    hasBool = true;
                    size = 1;
                }
                else if constexpr(std::is_floating_point_v<T>)
                {
                    hasReal = true;
                    size = sizeof(T)*8;
                }
                else if constexpr(std::is_integral_v<T>)
                {
                    if constexpr(std::is_signed_v<T>)
                    {
                        hasInt = true;
                    }
                    else
                    {
                        hasUint = true;
                    }
                    size = sizeof(T)*8;
                }
                else
                {
                    return;
                }

                minSize = std::min(size, minSize);
                maxSize = std::max(size, maxSize);
            });
        }

        if(hasReal)
        {
            if(maxSize >= 64)
            {
                return sink(std::add_pointer_t<real64>{});
            }
            return sink(std::add_pointer_t<real32>{});
        }

        if(hasUint)
        {
            if(maxSize >= 64)
            {
                return sink(std::add_pointer_t<uint64>{});
            }
            if(maxSize >= 32)
            {
                return sink(std::add_pointer_t<uint32>{});
            }
            if(maxSize >= 16)
            {
                return sink(std::add_pointer_t<uint16>{});
            }
            return sink(std::add_pointer_t<uint8>{});
        }

        if(hasInt)
        {
            if(maxSize >= 64)
            {
                return sink(std::add_pointer_t<int64>{});
            }
            if(maxSize >= 32)
            {
                return sink(std::add_pointer_t<int32>{});
            }
            if(maxSize >= 16)
            {
                return sink(std::add_pointer_t<int16>{});
            }
            return sink(std::add_pointer_t<int8>{});
        }

        if(hasBool)
        {
            return sink(std::add_pointer_t<bool_>{});
        }

        return sink(std::add_pointer_t<void>{});
    }
}
