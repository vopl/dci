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
#include "exec.hpp"
#include "../../eval.hpp"

namespace dci::module::ppn::node::rdb::instance::eval::function
{
    namespace idDetails
    {
        pql::Value apply1(const Args& args, const auto& f)
        {
            if(1 > args.size())
            {
                return {};
            }

            return args[0].data.visit([&](const auto& v) -> pql::Value
            {
                using T = std::decay_t<decltype(v)>;

                if constexpr(std::is_same_v<T, pql::id128> || std::is_same_v<T, pql::id256> || std::is_same_v<T, pql::id512>)
                {
                    return pql::Value{f(v.data)};
                }
                else
                {
                    return {};
                }
            });
        }

        pql::Value apply2(const Args& args, const auto& f)
        {
            if(2 > args.size())
            {
                return {};
            }

            return args[0].data.visit([&](const auto& id1) -> pql::Value
            {
                using T = std::decay_t<decltype(id1)>;

                if constexpr(std::is_same_v<T, pql::id128> || std::is_same_v<T, pql::id256> || std::is_same_v<T, pql::id512>)
                {
                    const auto& var = args[1].data;
                    if(!var.holds<T>())
                    {
                        return {};
                    }

                    return pql::Value{f(id1.data, var.get<T>().data)};
                }
                else
                {
                    return {};
                }
            });
        }

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::id_tip8>(const Args& args)
    {
        return idDetails::apply1(args, [](const auto id)
        {
            static_assert(1 == sizeof(id[0]));
            static_assert(1 <= id.size());
            return dci::utils::endian::l2n(*static_cast<const uint8*>(static_cast<const void*>(id.data()+id.size()-1)));
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::id_tip16>(const Args& args)
    {
        return idDetails::apply1(args, [](const auto id)
        {
            static_assert(1 == sizeof(id[0]));
            static_assert(2 <= id.size());
            return dci::utils::endian::l2n(*static_cast<const uint16*>(static_cast<const void*>(id.data()+id.size()-2)));
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::id_tip32>(const Args& args)
    {
        return idDetails::apply1(args, [](const auto id)
        {
            static_assert(1 == sizeof(id[0]));
            static_assert(4 <= id.size());
            return dci::utils::endian::l2n(*static_cast<const uint32*>(static_cast<const void*>(id.data()+id.size()-4)));
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::id_tip64>(const Args& args)
    {
        return idDetails::apply1(args, [](const auto id)
        {
            static_assert(1 == sizeof(id[0]));
            static_assert(8 <= id.size());
            return dci::utils::endian::l2n(*static_cast<const uint64*>(static_cast<const void*>(id.data()+id.size()-8)));
        });
    }
}
