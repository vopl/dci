// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/host.hpp>
#include <dci/cmt/task/owner.hpp>
#include <dci/utils/uri.hpp>
#include "ppn/transport/inproc.hpp"

namespace dci::module::ppn::transport::inproc
{
    using namespace dci;

    namespace api = idl::gen::ppn::transport::inproc;
    namespace apit = idl::gen::ppn::transport;
}
