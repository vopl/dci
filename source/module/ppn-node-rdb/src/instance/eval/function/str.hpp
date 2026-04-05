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
    pql::Value exec<pql::fun::str_concat>(const Args& args)
    {
        String res;

        for(std::size_t idx(0); idx<args.size(); ++idx)
        {
            res += cast<String>(args[idx]);
        }

        return {res};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::str_find>(const Args& args)
    {
        if(2 != args.size())
        {
            return {};
        }

        String::size_type pos = cast<String>(args[0]).find(cast<String>(args[1]));

        if(String::npos == pos)
        {
            return {-1};
        }

        return {static_cast<int>(pos)};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::str_match>(const Args& args)
    {
        if(2 != args.size())
        {
            return {};
        }

        return {std::regex_match(
                    cast<String>(args[0]),
                    std::regex{cast<String>(args[1])})};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::str_startsWith>(const Args& args)
    {
        if(2 != args.size())
        {
            return {};
        }

        return {0 == cast<String>(args[0]).find(cast<String>(args[1]))};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::str_endsWith>(const Args& args)
    {
        if(2 != args.size())
        {
            return {};
        }

        String s1 = cast<String>(args[0]);
        String s2 = cast<String>(args[1]);

        return {s1.size() - s2.size() == s1.find(s2)};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::str_substr>(const Args& args)
    {
        if(2 > args.size())
        {
            return {};
        }

        String s1 = cast<String>(args[0]);
        std::size_t pos = cast<size_t>(args[1]);
        std::size_t count = args.size() > 2 ? cast<size_t>(args[2]) : String::npos;

        if(pos >= s1.size())
        {
            return {String{}};
        }

        return {s1.substr(pos, count)};
    }
}
