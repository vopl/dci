// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/host.hpp>
#include <dci/cmt.hpp>
#include <dci/utils/endian.hpp>
#include <cmath>
#include <numeric>
#include <regex>
#include "ppn/node/rdb.hpp"

namespace dci::module::ppn::node::rdb
{
    using namespace dci;

    namespace api   = idl::gen::ppn::node::rdb;
    namespace query = idl::gen::ppn::node::rdb::query;
    namespace pql   = idl::gen::ppn::node::rdb::pql;
    namespace link  = idl::gen::ppn::node::link;
}
