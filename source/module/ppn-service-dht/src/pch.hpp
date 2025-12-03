// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/host.hpp>
#include <dci/mm/heap/allocable.hpp>
#include <dci/poll/waitableTimer.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>
#include "ppn/service/dht.hpp"
#include <memory>

namespace dci::module::ppn::service
{
    namespace node = idl::gen::ppn::node;
    namespace link = idl::gen::ppn::node::link;
    namespace api  = idl::gen::ppn::service::dht;
}
