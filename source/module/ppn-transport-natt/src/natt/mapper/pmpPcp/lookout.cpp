// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "lookout.hpp"
#include "../../mapper.hpp"
#include "../../addr.hpp"

namespace dci::module::ppn::transport::natt::mapper::pmpPcp
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Lookout::Lookout(Mapper* mapper, bool usePmp, bool usePcp)
        : mapper::netBased::Lookout<Lookout, Service>{mapper}
        , _usePmp{usePmp}
        , _usePcp{usePcp}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Lookout::~Lookout()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Lookout::usePmp() const
    {
        return _usePmp;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Lookout::usePcp() const
    {
        return _usePcp;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lookout::mcastReceived(Bytes&& data, const net::Endpoint& srvEp)
    {
        std::deque<net::IpAddress> linkAddresses;
        if(srvEp.holds<net::Ip4Endpoint>())
        {
            linkAddresses = linkAddressesFor(srvEp.get<net::Ip4Endpoint>().address);
        }
        else if(srvEp.holds<net::Ip6Endpoint>())
        {
            linkAddresses = linkAddressesFor(srvEp.get<net::Ip6Endpoint>().address);
        }

        for(const net::IpAddress& la : linkAddresses)
        {
            auto iter = _services.emplace(std::piecewise_construct_t{},
                                          std::tie(la, srvEp),
                                          std::forward_as_tuple(this, la, srvEp)).first;

            iter->second.mcastReceived(std::move(data), srvEp);
        }
    }

}
