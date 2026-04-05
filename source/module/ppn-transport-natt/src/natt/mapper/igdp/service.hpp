/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "pch.hpp"
#include "../netBased/service.hpp"
#include <pugixml.hpp>

namespace dci::module::ppn::transport::natt::mapper::igdp
{
    class Lookout;
    class Performer;
    class Service
        : public netBased::Service<Lookout, Service, Performer>
    {
    public:
        Service(Lookout* l, const net::IpAddress& clientAddr, const net::Endpoint& srvEp);
        ~Service() override;

        void mcastReceived(Bytes&& data, const net::Endpoint& ep);
        bool map(net::IpEndpoint& externalEp, uint16 internalPort, api::Protocol p, std::chrono::seconds lifetime);

    public:
        int performerPriority();
        void updateActivity();

        bool parseUdpResponse(Bytes&& data);

    public:
        Timepoint doRevealPayload();

    private:
        static net::stream::Channel<> xmlRequestSend(
                const net::Host<>& netHost,
                const String& soapServiceHostStr,
                const net::Endpoint& soapServiceEp,
                const String& path,
                const String& action,
                const String& post);

        pugi::xml_document xmlRequest(const String& path, const String& action = {}, const String& post = {});

    private:
        std::string     _soapServiceHostStr;
        std::string     _soapServicePathStr;
        net::Endpoint   _soapServiceEp;

    private:
        struct State : netBased::Service<Lookout, Service, Performer>::State
        {
            String _urn;
            String _SCPDURL;
            String _controlURL;
            String _eventSubURL;
            String _externalIp;

            State(const String& urn)
                : _urn{urn}
            {
            }
        };

        State  _v1 {"urn:schemas-upnp-org:service:WANIPConnection:1"};
        State  _v2 {"urn:schemas-upnp-org:service:WANIPConnection:2"};
    };
}
