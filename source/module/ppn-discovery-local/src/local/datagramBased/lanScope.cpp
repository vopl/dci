/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "lanScope.hpp"

namespace dci::module::ppn::discovery::local::datagramBased
{
    namespace
    {
        const net::Endpoint g_masterPoint{net::Ip4Endpoint{{}, 49073}};
        const net::Endpoint g_slavePoint{net::Ip4Endpoint{{}, 0}};
        const net::Endpoint g_broadcastPoint{net::Ip4Endpoint{{255,255,255,255}, 49073}};
        //const net::Endpoint g_broadcastPoint{net::Ip4Endpoint{{192,168,0,255}, 49073}};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    LanScope::LanScope(host::Manager* hostManager)
        : Base(hostManager)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    LanScope::~LanScope()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const std::string_view& LanScope::tag()
    {
        static const std::string_view res = "LanScope";
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const net::Endpoint& LanScope::masterPoint() const
    {
        return g_masterPoint;
    }

    const net::Endpoint& LanScope::slavePoint() const
    {
        return g_slavePoint;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void LanScope::tick()
    {
        dbgAssert(net());

        Bytes msg = makeMessage();
        LLOGI((isMaster()?"master":"slave")<<" sent "<<msg.size()<<" bytes");

        net()->send(std::move(msg), g_broadcastPoint);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool LanScope::addressAllowed(const transport::Address& a)
    {
        utils::URI<> uri;
        if(!utils::uri::parse(a.value, uri))
            return false;

        return std::visit([]<class Alt>(const Alt& alt)
                          {
                              if constexpr(std::is_same_v<utils::uri::TCP<>, Alt> || std::is_base_of_v<utils::uri::TCP<>, Alt>)
                              {
                                  return std::visit([]<class Host>(const Host& host)
                                  {
                                      if constexpr(std::is_same_v<utils::uri::networkNode::RegName<>, Host>)
                                          return true;
                                      if constexpr(std::is_same_v<utils::uri::networkNode::Ip4<>, Host>)
                                          return utils::ip::isCover(utils::ip::Scope::lan4, utils::ip::scope(host));
                                      if constexpr(std::is_same_v<utils::uri::networkNode::Ip6<>, Host>)
                                          return utils::ip::isCover(utils::ip::Scope::lan6, utils::ip::scope(host));
                                      return false;
                                  }, alt._auth._host);
                              }
                              return false;
                          }, uri);
    }
}
