// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "exec.hpp"

namespace dci::module::ppn::node::rdb::instance::eval::function
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::cmn_eqType>(const Args& args)
    {
        if(args.size() != 2) return {};
        return pql::Value{args[0].data.index() == args[1].data.index()};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::cmn_eq>(const Args& args)
    {
        if(args.size() != 2) return {};
        return pql::Value{args[0] == args[1]};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::cmn_ne>(const Args& args)
    {
        if(args.size() != 2) return {};
        return pql::Value{args[0] != args[1]};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::cmn_gt>(const Args& args)
    {
        if(args.size() != 2) return {};
        return pql::Value{args[0] > args[1]};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::cmn_lt>(const Args& args)
    {
        if(args.size() != 2) return {};
        return pql::Value{args[0] < args[1]};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::cmn_ge>(const Args& args)
    {
        if(args.size() != 2) return {};
        return pql::Value{args[0] >= args[1]};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    pql::Value exec<pql::fun::cmn_le>(const Args& args)
    {
        if(args.size() != 2) return {};
        return pql::Value{args[0] <= args[1]};
    }
}
