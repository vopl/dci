// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"

namespace dci::module::ppn::transport::net
{
    idl::gen::net::Endpoint address2Endpoint(idl::gen::net::Host<>& host, const apit::Address& target);
}
