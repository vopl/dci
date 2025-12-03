// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "address2Endpoint.hpp"

namespace dci::module::ppn::transport::net
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    idl::gen::net::Endpoint address2Endpoint(idl::gen::net::Host<>& host, const apit::Address& target)
    {
        utils::URI<> uri;
        if(!utils::uri::parse(target.value, uri))
            throw api::BadAddress(target.value);

        return std::visit([&]<class Alt>(const Alt& alt)
                          {
                              idl::gen::net::Endpoint ep {};

                              if constexpr(std::is_same_v<utils::uri::TCP<>, Alt>)
                              {
                                  idl::gen::net::IpEndpoint epIp = host->resolveIp(utils::uri::hostPort(alt)).value();
                                  if(epIp.holds<idl::gen::net::Ip4Endpoint>())
                                      ep = epIp.get<idl::gen::net::Ip4Endpoint>();
                                  else if(epIp.holds<idl::gen::net::Ip6Endpoint>())
                                      ep = epIp.get<idl::gen::net::Ip6Endpoint>();
                              }
                              else if constexpr(std::is_same_v<utils::uri::TCP4<>, Alt>)
                                  ep = host->resolveIp4(utils::uri::hostPort(alt)).value();
                              else if constexpr(std::is_same_v<utils::uri::TCP6<>, Alt>)
                                  ep = host->resolveIp6(utils::uri::hostPort(alt)).value();
                              else if constexpr(std::is_same_v<utils::uri::Local<>, Alt>)
                              {
                                  std::string authority = utils::uri::hostPort(alt);
                                  if(!authority.empty())
                                  {
                                      authority.insert(0, 1, '\0');
                                      ep = idl::gen::net::LocalEndpoint{std::move(authority)};
                                  }
                                  else
                                      ep = idl::gen::net::LocalEndpoint{};
                              }
                              else
                                  throw api::BadAddress(target.value);

                              return ep;
                          }, uri);
    }
}
