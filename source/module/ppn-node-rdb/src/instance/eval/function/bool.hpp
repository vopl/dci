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
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::bool_not>(const Args& args)
    {
        if(args.size() < 1) return {true};
        return {!cast<bool>(args[0])};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::bool_and>(const Args& args)
    {
        if(args.size() < 1) return {false};

        bool res = cast<bool>(args[0]);
        for(std::size_t idx(1); idx<args.size(); ++idx)
        {
            res = res && cast<bool>(args[idx]);
        }

        return {res};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::bool_or>(const Args& args)
    {
        if(args.size() < 1) return {false};

        bool res = cast<bool>(args[0]);
        for(std::size_t idx(1); idx<args.size(); ++idx)
        {
            res = res || cast<bool>(args[idx]);
        }

        return {res};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::bool_xor>(const Args& args)
    {
        if(args.size() < 1) return {false};

        bool res = cast<bool>(args[0]);
        for(std::size_t idx(1); idx<args.size(); ++idx)
        {
            res = !res != !cast<bool>(args[idx]);
        }

        return {res};
    }
}
