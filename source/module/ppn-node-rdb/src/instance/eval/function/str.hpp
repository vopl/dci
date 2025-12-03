// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
