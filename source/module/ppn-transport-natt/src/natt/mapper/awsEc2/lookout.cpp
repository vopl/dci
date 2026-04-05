/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "lookout.hpp"
#include "performer.hpp"
#include "../../mapper.hpp"
#include "../../addr.hpp"

namespace dci::module::ppn::transport::natt::mapper::awsEc2
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Lookout::Lookout(Mapper* mapper)
        : mapper::Lookout{mapper}
    {
        _name = "awsEc2";

        cmt::spawn() += _tol * [this]{reveal();};
        _revealTicker.start();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Lookout::~Lookout()
    {
        _tol.stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    PerformerBlank Lookout::candidate(const apit::Address& internal)
    {
        api::Protocol protocol;
        net::IpEndpoint internalEp;

        if(!addr::fromString(internal.value, protocol, internalEp))
        {
            return {};
        }

        if(!internalEp.holds<net::Ip4Endpoint>())
        {
            return {};
        }

        const net::Ip4Endpoint& internalEp4 = internalEp.get<net::Ip4Endpoint>();

        if(internalEp4.address != _internal)
        {
            return {};
        }

        return PerformerBlank
        {
            PerformerBlank::Builder
            {
                [=,this]{return PerformerPtr{new Performer{this, internalEp4}, [](mapper::Performer*p){delete static_cast<Performer*>(p);}};}
            },
            50
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const net::Ip4Address& Lookout::internal() const
    {
        return _internal;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const net::Ip4Address& Lookout::external() const
    {
        return _external;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Lookout::revision() const
    {
        return _revision;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Waitable& Lookout::changed()
    {
        return _changed;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lookout::reveal()
    {
        if(_revealDepth) return;
        ++_revealDepth;
        utils::AtScopeExit se{[this]
        {
            --_revealDepth;
        }};

        try
        {
            net::Host<> host = mapper()->hostManager()->createService<net::Host<>>().value();

            net::stream::Client<> client = host->streamClient().value();

            net::Ip4Address internal = requestIp(client, "/latest/meta-data/local-ipv4");
            net::Ip4Address external = requestIp(client, "/latest/meta-data/public-ipv4");

            if(internal != _internal || external != _external)
            {
                _internal = internal;
                _external = external;

                ++_revision;
                _changed.raise();
                if(!activate(this))
                {
                    mapper()->lookoutChanged(this);
                }
            }
        }
        catch(const cmt::task::Stop&)
        {
            deactivate(this);
        }
        catch(const net::TimedOut&)
        {
            _internal = {};
            _external = {};
            ++_revision;
            _changed.raise();
            deactivate(this);
        }
        catch(...)
        {
            LOGW(_name<<": reveal failed: "<<exception::toString(std::current_exception()));
            _internal = {};
            _external = {};
            ++_revision;
            _changed.raise();
            deactivate(this);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    net::Ip4Address Lookout::requestIp(net::stream::Client<> client, const std::string& path)
    {
        net::stream::Channel<> ch = client->connect(net::Ip4Endpoint{{169,254,169,254}, 80}).value();

        ch->send("GET "+path+" HTTP/1.1\r\n"
                "Host: 169.254.169.254\r\n"
                "User-Agent: " dciModuleName "\r\n"
                "Connection: close\r\n"
                "\r\n");

        ch->startReceive();

        utils::S2f<> closedf {ch->closed()};

        String content;
        size_t headersSize {};
        size_t contentSize {};

        for(;;)
        {
            utils::S2f receivedf = ch->received();

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

        ch->close();

        if(!contentSize)
        {
            contentSize = content.size() - headersSize;
        }

        std::string_view ip4Str{content.c_str() + headersSize, contentSize};

        net::Ip4Address ip4{};
        if(!utils::ip::fromString(ip4Str, ip4.octets))
        {
            LOGW(_name<<": bad ip address received: "<<content);
            return {};
        }

        return ip4;
    }
}
