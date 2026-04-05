/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "service.hpp"
#include "lookout.hpp"
#include "performer.hpp"
#include "../../addr.hpp"
#include "../../mapper.hpp"

namespace dci::module::ppn::transport::natt::mapper::igdp
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Service::Service(Lookout* l, const net::IpAddress& clientAddr, const net::Endpoint& srvEp)
        : netBased::Service<Lookout, Service, Performer>{l, clientAddr, srvEp}
    {
        _name = "igdp at " + addr::toString(_srvEp);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Service::~Service()
    {
        _revealTicker.stop();
        _l->deactivate(this);
        _clientSol.flush();
        _client.reset();
        _tol.stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Service::mcastReceived(Bytes&& data, const net::Endpoint& ep)
    {
        (void)ep;

        if(parseUdpResponse(std::move(data)))
        {
            _revealTicker.interval(_timeoutRevealUrgent);
            _revealTicker.start();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Service::map(net::IpEndpoint& externalEp, uint16 internalPort, api::Protocol p, std::chrono::seconds lifetime)
    {
        auto add = [&](State& state, const String& method)
        {
            uint16 externalPort;
            if(externalEp.holds<net::Ip4Endpoint>()) externalPort = externalEp.get<net::Ip4Endpoint>().port;
            else                                     externalPort = externalEp.get<net::Ip6Endpoint>().port;

            for(std::size_t i{0}; i<10; ++i)
            {

                std::string protocolStr = (api::Protocol::udp == p ? "UDP" : "TCP");

                pugi::xml_document resp = xmlRequest(state._controlURL, state._urn+"#"+method,
                                    "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
                                    "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                                        "<s:Body>"
                                            "<u:"+method+" xmlns:u=\"" + state._urn + "\">"

                                                //"<NewRemoteHost>"+state._externalIp+"</NewRemoteHost>"
                                                "<NewRemoteHost></NewRemoteHost>" // пустая строка как внешний адрес

                                                "<NewExternalPort>" + std::to_string(externalPort) + "</NewExternalPort>"
                                                "<NewProtocol>"+protocolStr+"</NewProtocol>"
                                                "<NewInternalPort>" + std::to_string(internalPort) + "</NewInternalPort>"
                                                "<NewInternalClient>" + addr::toString(_clientAddr) + "</NewInternalClient>"
                                                "<NewEnabled>1</NewEnabled>"
                                                "<NewPortMappingDescription>" + (dciModuleName " " + std::to_string(externalPort) + " " + protocolStr) + "</NewPortMappingDescription>"
                                                "<NewLeaseDuration>" + std::to_string(lifetime.count()) + "</NewLeaseDuration>"
                                            "</u:"+method+">"
                                        "</s:Body>"
                                    "</s:Envelope>");

                String externalPortStr = resp.select_node("//NewReservedPort").node().text().get();
                if(!externalPortStr.empty() && std::errc{} == std::from_chars(externalPortStr.c_str(), externalPortStr.c_str()+externalPortStr.size(), externalPort).ec)
                {
                    utils::ip::Address4 ip4;
                    if(utils::ip::fromString(state._externalIp, ip4))
                    {
                        externalEp = net::Ip4Endpoint{{ip4}, externalPort};
                        return true;
                    }

                    utils::ip::Address6 ip6;
                    if(utils::ip::fromString(state._externalIp, ip6))
                    {
                        externalEp = net::Ip6Endpoint{{{ip6}, 0}, externalPort};
                        return true;
                    }
                }

                externalPort = std::experimental::randint(uint16{1024}, uint16{65535});
            }

            state.fixBan();
            return false;
        };

        auto del = [&](State& state)
        {
            uint16 externalPort;
            if(externalEp.holds<net::Ip4Endpoint>()) externalPort = externalEp.get<net::Ip4Endpoint>().port;
            else                                     externalPort = externalEp.get<net::Ip6Endpoint>().port;

            net::Host<> netHost = _l->netHost();
            std::string soapServiceHostStr = _soapServiceHostStr;
            net::Endpoint soapServiceEp = _soapServiceEp;
            String control = state._controlURL;
            String urn = state._urn;
            String externalIp = state._externalIp;

            cmt::spawn() += [=, stopLocker=_l->mapper()->hostModuleStopLocker()]
            {
                try
                {
                    net::stream::Channel<> ch = xmlRequestSend(
                                    netHost, soapServiceHostStr, soapServiceEp, control, urn+"#DeletePortMapping",
                                    "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
                                    "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                                        "<s:Body>"
                                            "<u:DeletePortMapping xmlns:u=\"" + urn + "\">"

                                                //"<NewRemoteHost>"+externalIp+"</NewRemoteHost>"
                                                "<NewRemoteHost></NewRemoteHost>"

                                                "<NewExternalPort>" + std::to_string(externalPort) + "</NewExternalPort>"
                                                "<NewProtocol>"+(api::Protocol::udp == p ? "UDP" : "TCP")+"</NewProtocol>"
                                            "</u:DeletePortMapping>"
                                        "</s:Body>"
                                    "</s:Envelope>");

                    ch->startReceive();

                    utils::S2f<> closedf{ch->closed()};
                    utils::S2f<Bytes> receivedf{ch->received()};

                    cmt::waitAny(closedf, receivedf);
                }
                catch(...)
                {
                    //ignore
                }
            };

            return true;
        };

        if(_v2.ok() && !_v2._controlURL.empty() && !_v2._externalIp.empty())
        {
            if(lifetime.count())    return add(_v2, "AddAnyPortMapping");
            else                    return del(_v2);
        }

        if(_v1.ok() && !_v1._controlURL.empty() && !_v1._externalIp.empty())
        {
            if(lifetime.count())    return add(_v1, "AddAnyPortMapping");
            else                    return del(_v1);
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    int Service::performerPriority()
    {
        static constexpr int maximal = 10;

        int ban = std::min(_v1.ok() ? static_cast<int>(_v1._ban) : maximal,
                           _v2.ok() ? static_cast<int>(_v2._ban) : maximal);

        return std::clamp(maximal-ban, 0, maximal);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Service::updateActivity()
    {
        if(!_v1.ok() && !_v2.ok())
        {
            _l->deactivate(this);
        }
        else
        {
            _l->activate(this);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Service::parseUdpResponse(Bytes&& data)
    {
        using namespace std::literals::string_view_literals;

        String content = data.toString();
        String::size_type headersEnd = content.find("\r\n\r\n");
        if(content.npos == headersEnd)
        {
            LOGW(_name<<": abnormal igdp recv: "<<content);
            return false;
        }

        std::string_view headers(content.data(), headersEnd);

        if(headers.starts_with("M-SEARCH * HTTP/"sv))
        {
            //ignore
            return false;
        }

        if(headers.starts_with("NOTIFY * HTTP/"sv) || headers.starts_with("HTTP/"sv))
        {
            static const std::regex regex("\r\nlocation:\\s*http://([^/]*)(/.*)\\s*\r\n", std::regex::ECMAScript | std::regex::icase | std::regex::optimize);

            std::cmatch match;
            if(!std::regex_search(headers.begin(), headers.end(), match, regex))
            {
                //no location found
                return false;
            }

            std::string soapServiceHostStr = match[1];
            std::string soapServicePathStr = match[2];

            if(soapServiceHostStr == _soapServiceHostStr && soapServicePathStr == _soapServicePathStr)
            {
                //LOGI(_name<<" confirmed host: "<<_soapServiceHostStr<<", path: "<<_soapServicePathStr<<"\n"<<content);
                return true;
            }

            _soapServiceHostStr = soapServiceHostStr;
            _soapServicePathStr = soapServicePathStr;

            //LOGI(_name<<" found host: "<<_soapServiceHostStr<<", path: "<<_soapServicePathStr);

            net::Ip4Endpoint soapServiceEp4;
            if(utils::ip::fromString(soapServiceHostStr, soapServiceEp4.address.octets, soapServiceEp4.port))
            {
                _soapServiceEp = soapServiceEp4;
            }
            else
            {
                net::Ip6Endpoint soapServiceEp6;
                if(utils::ip::fromString(soapServiceHostStr, soapServiceEp6.address.octets, soapServiceEp6.address.linkId, soapServiceEp6.port))
                {
                    _soapServiceEp = soapServiceEp6;
                }
                else
                {
                    LOGW(_name<<": abnormal igdp recv: "<<content);
                    return false;
                }
            }

            return true;
        }
        else
        {
            LOGW(_name<<": abnormal igdp recv: "<<content);
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Service::Timepoint Service::doRevealPayload()
    {
        if(_soapServiceHostStr.empty() || _soapServicePathStr.empty() || _soapServiceEp == net::Endpoint{})
        {
            auto requestMaker1 = [this]
            {
                String request = "M-SEARCH * HTTP/1.1\r\n"
                                 "HOST: " + addr::toString(_clientEp) +"\r\n"
                                 "MAN: \"ssdp:discover\"\r\n"
                                 "MX: 2\r\n"
                                 "ST: "+_v1._urn+"\r\n"
                                 "\r\n";
                return Bytes{request};
            };

            auto requestMaker2 = [this]
            {
                String request = "M-SEARCH * HTTP/1.1\r\n"
                                 "HOST: " + addr::toString(_clientEp) +"\r\n"
                                 "MAN: \"ssdp:discover\"\r\n"
                                 "MX: 2\r\n"
                                 "ST: "+_v2._urn+"\r\n"
                                 "\r\n";
                return Bytes{request};
            };

            auto responseHandler = [this](Bytes&& data)
            {
                if(parseUdpResponse(std::move(data)))
                {
                    return InteractResult::success;
                }

                return InteractResult::null;
            };

            interact(requestMaker1, responseHandler);
            interact(requestMaker2, responseHandler);
        }

        if(_soapServiceHostStr.empty() || _soapServicePathStr.empty() || _soapServiceEp == net::Endpoint{})
        {
            return Clock::now() + _timeoutRevealAfterFail;
        }

        pugi::xml_document resp = xmlRequest(_soapServicePathStr);

        auto findServices = [&](const pugi::xml_node& device)
        {
            auto findServicesImpl = [&](const pugi::xml_node& device, const auto& self) -> void
            {
                for(const pugi::xpath_node& s : device.select_nodes("serviceList/service"))
                {
                    pugi::xml_node service = s.node();

                    if(service.child_value("serviceType") == _v1._urn)
                    {
                        _v1._SCPDURL = service.child_value("SCPDURL");
                        _v1._controlURL = service.child_value("controlURL");
                        _v1._eventSubURL = service.child_value("eventSubURL");
                    }
                    else if(service.child_value("serviceType") == _v2._urn)
                    {
                        _v2._SCPDURL = service.child_value("SCPDURL");
                        _v2._controlURL = service.child_value("controlURL");
                        _v2._eventSubURL = service.child_value("eventSubURL");
                    }
                }

                for(const pugi::xpath_node& s : device.select_nodes("deviceList/device"))
                {
                    self(s.node(), self);
                }
            };

            findServicesImpl(device, findServicesImpl);
        };

        for(const pugi::xpath_node& s : resp.select_nodes("/root/device"))
        {
            findServices(s.node());
        }

        auto getExternalIPAddress = [&](State& state)
        {
            if(state._controlURL.empty())
            {
                state.fixFailed(Clock::now());

                if(!state._externalIp.empty())
                {
                    state._externalIp.clear();
                    _serviceChanged.raise();
                }
                return;
            }

            Timepoint start = Clock::now();

            resp = xmlRequest(state._controlURL, state._urn+"#GetExternalIPAddress",
                                "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
                                "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                                    "<s:Body>"
                                        "<u:GetExternalIPAddress xmlns:u=\""+state._urn+"\"/>"
                                    "</s:Body>"
                                "</s:Envelope>");

            String externalIp = resp.select_node("//NewExternalIPAddress").node().text().get();
            if(externalIp != state._externalIp)
            {
                state._externalIp = std::move(externalIp);
                LOGI(_name<<" http://"<<_soapServiceHostStr<<state._controlURL<<", "<<state._urn<<", external: "<<state._externalIp);

                _serviceChanged.raise();
            }

            state.fixTried(start);
            if(state._externalIp.empty())
            {
                state.fixFailed(Clock::now());
            }
            else
            {
                state.fixFound(Clock::now());
            }
        };

        getExternalIPAddress(_v1);
        getExternalIPAddress(_v2);

        updateActivity();

        if(_v1.ok() && _v2.ok())
        {
            return std::min(_v1.nextReveal(), _v2.nextReveal());
        }

        if(_v1.ok())
        {
            return _v1.nextReveal();
        }

        if(_v2.ok())
        {
            return _v2.nextReveal();
        }

        return Clock::now() + _timeoutRevealAfterFail;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    net::stream::Channel<> Service::xmlRequestSend(
            const net::Host<>& netHost,
            const String& soapServiceHostStr,
            const net::Endpoint& soapServiceEp,
            const String& path,
            const String& action,
            const String& post)
    {
        std::string headers;

        if(post.empty())
        {
            headers = "GET " + path + " HTTP/1.1\r\n";
        }
        else
        {
            headers = "POST " + path + " HTTP/1.1\r\n"
                      "Content-Type: text/xml; charset=\"utf-8\"\r\n"
                      "Content-Length: " + std::to_string(post.size()) + "\r\n";
        }

        if(!action.empty())
        {
            headers += "SOAPAction: " + action + "\r\n";
        }

        headers += "Host: " + soapServiceHostStr + "\r\n"
                   "User-agent: " + dciModuleName + "\r\n"
                   "Connection: close\r\n"
                   "\r\n";

        net::stream::Client<> client = netHost->streamClient().value();
        net::stream::Channel<> sch = client->connect(soapServiceEp).value();

        sch->send(headers + post);

        return sch;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    pugi::xml_document Service::xmlRequest(const String& path, const String& action, const String& post)
    {
        net::stream::Channel<> sch = xmlRequestSend(_l->netHost(), _soapServiceHostStr, _soapServiceEp, path, action, post);

        sch->startReceive();

        utils::S2f<> closedf {sch->closed()};

        String content;
        size_t headersSize {};
        size_t contentSize {};

        for(;;)
        {
            utils::S2f receivedf = sch->received();

            if(0 == cmt::waitAny(closedf, receivedf))
            {
                break;
            }

            Bytes chunk = receivedf.value();
            content += chunk.toString();

            if(!headersSize)
            {
                headersSize = content.find("\r\n\r\n");
                if(String::npos == headersSize)
                {
                    headersSize = 0;
                    continue;
                }
                headersSize += 4;
            }

            if(!contentSize)
            {
                static const std::regex regex("\r\nContent-Length:\\s*(\\d+)\\s*\r\n", std::regex::ECMAScript | std::regex::icase | std::regex::optimize);

                std::cmatch match;
                if(!std::regex_search(content.c_str(), content.c_str()+headersSize, match, regex))
                {
                    continue;
                }

                if(std::errc{} != std::from_chars(match[1].first, match[1].second, contentSize).ec)
                {
                    continue;
                }
            }

            if(content.size() >= headersSize + contentSize)
            {
                break;
            }
        }

        sch->close();

        if(!contentSize)
        {
            contentSize = content.size() - headersSize;
        }

        contentSize = std::min(contentSize, content.size() - headersSize);

        if(!contentSize)
        {
            return {};
        }

        pugi::xml_document res;
        pugi::xml_parse_result loadres = res.load_buffer(content.c_str() + headersSize, contentSize);

        if(!loadres)
        {
            LOGW(_name<<": bad xml: "<<loadres.description());
            LOGW(_name<<": full response: "<<content);
            return {};
        }

        return res;
    }
}
