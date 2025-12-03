// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "endpoint2Address.hpp"

namespace dci::module::ppn::transport::net
{
    apit::Address endpoint2Address(const idl::gen::net::Endpoint& ep)
    {
        apit::Address res;

        if(ep.holds<idl::gen::net::NullEndpoint>())
        {
            res.value = "null://";
            return res;
        }
        else if(ep.holds<idl::gen::net::Ip4Endpoint>())
        {
            const idl::gen::net::Ip4Endpoint& ep4 = ep.get<idl::gen::net::Ip4Endpoint>();
            res.value = "tcp4://" + utils::ip::toString(ep4.address.octets, ep4.port);
            return res;
        }
        else if(ep.holds<idl::gen::net::Ip6Endpoint>())
        {
            const idl::gen::net::Ip6Endpoint& ep6 = ep.get<idl::gen::net::Ip6Endpoint>();
            res.value = "tcp6://" + utils::ip::toString(ep6.address.octets, ep6.address.linkId, ep6.port);
            return res;
        }
        else if(ep.holds<idl::gen::net::LocalEndpoint>())
        {
            const idl::gen::net::LocalEndpoint& epl = ep.get<idl::gen::net::LocalEndpoint>();
            res.value = "local://" + epl.address.substr(1);
        }
        else
        {
            dbgFatal("never here");
        }

        return res;
    }
}
