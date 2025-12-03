// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "utils/args.hpp"

namespace dci::module::ppn::node::rdb::instance::eval::function
{
    using namespace utils;

    template <pql::fun>
    pql::Value exec(const Args& args)
    {
        (void)args;
        dbgWarn("not implemented");
        return pql::Value{};
    }
}
