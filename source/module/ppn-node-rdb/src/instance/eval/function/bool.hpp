// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
