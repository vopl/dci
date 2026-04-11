/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include <expected>
#include "io.hpp"
#include "connection.hpp"
#include "site.hpp"
#include "../agent.hpp"
#include "../enumSupport.hpp"
#include "rcptr.hpp"

namespace dci::module::www::agent
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Io::Io(Agent* agent, const api::http::client::Cookies<>& cookies, api::agent::io::Request&& request)
        : _agent{agent}
        , _cookies{cookies}
        , _request{std::move(request)}
    {
        _responsePromise.canceled() += _sol * [this]
        {
            auto holder{rcptr()};

            if(_connection)
                _connection->ioCancelled(rcptr());
            else if(_site)
                _site->ioCancelled(rcptr());
            else if(_agent)
                _agent->ioCancelled(rcptr());
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Io::~Io()
    {
        _sol.flush();
        _tol.flush();
        if(!_responsePromise.resolved())
            _responsePromise.resolveException(exception::buildInstance<api::agent::Stopped>());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<api::agent::io::Response> Io::future()
    {
        return _responsePromise.future();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Io::done()
    {
        return _responsePromise.resolved();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::fail(const ExceptionPtr& fail)
    {
        log([&]{ return "io failed: " + exception::toString(fail); });

        _sol.flush();
        _tol.flush();
        _httpResponse.reset();
        if(!_responsePromise.resolved())
        {
            _responsePromise.resolveException(std::move(fail));

            if(_connection)
                _connection->ioFailed(rcptr());
            else if(_site)
                _site->ioFailed(rcptr());
            else if(_agent)
                _agent->ioFailed(rcptr());
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::expected<site::Endpoint, ExceptionPtr> Io::calculateSiteEndpoint()
    {
        dbgAssert(!_responsePromise.resolved());

        utils::URI<std::string_view> uriParsed;
        if(!utils::uri::parse(_request.uri, uriParsed))
        {
            ExceptionPtr error = exception::buildInstance<api::agent::BadUri>(_request.uri);
            fail(error);
            return std::unexpected{std::move(error)};
        }

        bool ok = std::visit([&]<class T>(T&& uri)
        {
            if constexpr(std::is_same_v<utils::uri::HTTP<std::string_view>, T> || std::is_same_v<utils::uri::HTTPS<std::string_view>, T>)
            {
                _uriParsed = std::move(uri);
                return true;
            }
            return false;
        }, std::move(uriParsed));
        if(!ok)
        {
            ExceptionPtr error = exception::buildInstance<api::agent::BadUriScheme>(utils::uri::scheme(_uriParsed));
            fail(error);
            return std::unexpected{std::move(error)};
        }

        using namespace std::literals;
        site::Endpoint siteEndpoint;
        siteEndpoint._secure = "https"sv == _uriParsed._scheme;

        std::string_view hostView;
        ok = std::visit([&]<class T>(const T& host)
        {
            hostView = host;
            if constexpr(std::is_same_v<utils::uri::networkNode::Ip4<std::string_view>, T> ||
                         std::is_same_v<utils::uri::networkNode::Ip6<std::string_view>, T>)
            {
                siteEndpoint._host.reserve(host.size() + 1 + 5);
                siteEndpoint._host = host;
                return true;
            }
            if constexpr(std::is_same_v<utils::uri::networkNode::RegName<std::string_view>, T>)
            {
                siteEndpoint._host.reserve(host.size()*3 + 1 + 5);
                siteEndpoint._host = host;
                switch(utils::dns::canonicalize(siteEndpoint._host))
                {
                    case utils::dns::CanonicalizeResult::unneeded:
                    case utils::dns::CanonicalizeResult::ok:
                        return true;
                    default:
                        return false;
                }
            }
            return false;
        }, _uriParsed._auth._networkNode._host);
        if(!ok)
        {
            ExceptionPtr error = exception::buildInstance<api::agent::BadUriHost>(hostView);
            fail(error);
            return std::unexpected{std::move(error)};
        }

        if(_uriParsed._auth._networkNode._port)
        {
            std::string_view portView{*_uriParsed._auth._networkNode._port};
            auto [end, errc] = std::from_chars(portView.data(), portView.data() + portView.size(), siteEndpoint._port);
            if(errc != std::errc{})
            {
                ExceptionPtr error = exception::buildInstance<api::agent::BadUriPort>(portView);
                fail(error);
                return std::unexpected{std::move(error)};
            }
        }
        else
        {
            if(siteEndpoint._secure)
                siteEndpoint._port = 443;
            else
                siteEndpoint._port = 80;
        }

        return siteEndpoint;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::setAgent(Agent* agent)
    {
        dbgAssert(!_agent == !!agent);
        _agent = agent;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::setSite(Site* site)
    {
        dbgAssert(!_site == !!site);
        _site = site;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::setConnection(Connection* connection)
    {
        dbgAssert(!_connection == !!connection);
        _connection = connection;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::perform(const api::http::client::Channel<>& httpChannel)
    {
        _started = true;

        if(_responsePromise.resolved())
            return;

        api::http::client::Request<> httpRequest;
        dbgAssert(!httpRequest);
        httpRequest.init();

        dbgAssert(!_httpResponse);
        _httpResponse.init();

        {
            _httpResponse->firstLine() += _sol * [this](api::http::firstLine::Version version, api::http::firstLine::StatusCode statusCode, primitives::String&& statusText)
            {
                auto holder{rcptr()};

                if(isOrphan())
                    return;

                log([&]{ return "! " + std::string{enumSupport::toString(version).value_or("?")} + " " + std::to_string(statusCode) + " " + statusText; });
                if(isOrphan())
                    return;

                _responseAccumuler.version = version;
                _responseAccumuler.statusCode = statusCode;
                _responseAccumuler.statusText = std::move(statusText);
            };

            _httpResponse->headers() += _sol * [this](primitives::List<api::http::Header>&& headers, bool done)
            {
                auto holder{rcptr()};

                if(isOrphan())
                    return;

                logHeaders(headers, done, "!");
                if(isOrphan())
                    return;

                _responseAccumuler.headers.insert(_responseAccumuler.headers.end(), std::move_iterator{headers.begin()}, std::move_iterator{headers.end()});
            };

            _httpResponse->data() += _sol * [this](Bytes&& data, bool done)
            {
                auto holder{rcptr()};

                if(isOrphan())
                    return;

                logData(data, done, "!");
                if(isOrphan())
                    return;

                _responseAccumuler.data.end().write(std::move(data));
            };

            _httpResponse->done() += _sol * [this]()
            {
                auto holder{rcptr()};

                if(isOrphan())
                    return;

                log([&]{ return "! done"; });
                if(isOrphan())
                    return;

                onResponseDone();
            };

            _httpResponse->failed() += _sol * [this](ExceptionPtr e)
            {
                auto holder{rcptr()};

                if(isOrphan())
                    return;

                fail(exception::buildInstance<api::agent::HttpFailed>(e));
            };

            _httpResponse->closed() += _sol * [this]()
            {
                auto holder{rcptr()};

                if(isOrphan())
                    return;

                log([&]{ return "! closed"; });
                if(isOrphan())
                    return;

                if(!_responsePromise.resolved())
                {
                    _responsePromise.resolveException(exception::buildInstance<api::agent::HttpClosed>());
                    if(isOrphan())
                        return;
                }

                _connection->ioWantClose(rcptr());
            };

            httpRequest->failed() += _sol * [this](ExceptionPtr e)
            {
                auto holder{rcptr()};

                if(isOrphan())
                    return;

                fail(exception::buildInstance<api::agent::HttpFailed>(e));
            };

            httpRequest->closed() += _sol * [this]()
            {
                auto holder{rcptr()};

                if(isOrphan())
                    return;
                log([&]{ return "? closed"; });
                if(isOrphan())
                    return;

                if(!_responsePromise.resolved())
                {
                    _responsePromise.resolveException(exception::buildInstance<api::agent::HttpClosed>());
                    if(isOrphan())
                        return;
                }

                _connection->ioWantClose(rcptr());
            };
        }

        httpChannel->io(httpRequest.opposite(), _httpResponse.opposite());
        if(_responsePromise.resolved())
            return;
        if(isOrphan())
            return;

        _httpResponse->setupDataProcessing(
                    api::http::message::tunable::Compression::byHeaders,
                    api::http::message::tunable::Compression::byHeaders,
                    api::http::message::tunable::Encoding::byHeaders);
        if(isOrphan())
            return;

        if(_responsePromise.resolved())
            return;
        if(isOrphan())
            return;

        cmt::spawn() += _tol * [this, httpRequest=std::move(httpRequest)]()
        {
            auto holder{rcptr()};

            if(isOrphan())
                return;

            bool withBody{};
            switch(_request.method)
            {
            case api::http::firstLine::Method::DELETE:
            case api::http::firstLine::Method::POST:
            case api::http::firstLine::Method::PUT:
            case api::http::firstLine::Method::PATCH:
                withBody = true;

            default:
                break;
            }

            bool withCompression{};

            if(withBody)
            {
                withCompression = _request.body.size() > 1024;
                if(withCompression)
                {
                    httpRequest->setupDataProcessing(
                            api::http::message::tunable::Compression::none,
                            api::http::message::tunable::Compression::br,
                            api::http::message::tunable::Encoding::chunked);
                    if(isOrphan())
                        return;
                }
            }

            {
                String uri4Request;
                if(_uriParsed._path.empty())
                    uri4Request = "/";
                else
                    uri4Request = _uriParsed._path;
                if(_uriParsed._query)
                {
                    uri4Request += '?';
                    uri4Request += *_uriParsed._query;
                }

                api::http::firstLine::Version httpVersion = api::http::firstLine::Version::HTTP_1_1;
                log([&]{ return "? " + std::string{enumSupport::toString(_request.method).value_or("?")} + " " + uri4Request + " " + std::string{enumSupport::toString(httpVersion).value_or("?")}; });
                if(isOrphan())
                    return;
                httpRequest->firstLine(_request.method, std::move(uri4Request), httpVersion);
                if(isOrphan())
                    return;
            }

            {
                dbgAssert(_site);
                const site::Endpoint& siteEndpoint = _site->endpoint();

                primitives::List<api::http::Header> headers;
                headers.emplace_back(api::http::header::KeyRecognized::Host, siteEndpoint._host);
                if(withBody)
                {
                    if(withCompression)
                        headers.emplace_back(api::http::header::KeyRecognized::Transfer_Encoding, "br, chunked");
                    else
                        headers.emplace_back(api::http::header::KeyRecognized::Content_Length, std::to_string(_request.body.size()));
                }
                headers.emplace_back(api::http::header::KeyRecognized::TE, "deflate, gzip, zstd, br, chunked");
                headers.emplace_back(api::http::header::KeyRecognized::Accept_Encoding, "deflate, gzip, zstd, br");

                if(_cookies)
                {
                    bool isHttp = true;
                    primitives::List<String> cookies;
                    try
                    {
                        cookies = _cookies->toRequest(siteEndpoint._host, _uriParsed._path, siteEndpoint._secure, isHttp).value();
                        if(isOrphan())
                            return;
                    }
                    catch(const cmt::task::Stop&)
                    {
                        fail(exception::buildInstance<api::agent::Stopped>());
                        return;
                    }
                    catch(...)
                    {
                        fail(exception::buildInstance<api::agent::CookieFailed>(std::current_exception()));
                        return;
                    }
                    for(String& cookie : cookies)
                        headers.emplace_back(api::http::header::KeyRecognized::Cookie, std::move(cookie));
                }

                bool hasExtraHeaders = !_request.headers.empty();
                logHeaders(headers, !hasExtraHeaders, "?");
                if(isOrphan())
                    return;
                httpRequest->headers(std::move(headers), !hasExtraHeaders);
                if(isOrphan())
                    return;

                if(hasExtraHeaders)
                {
                    logHeaders(_request.headers, true, "?");
                    if(isOrphan())
                        return;
                    httpRequest->headers(std::move(_request.headers), true);
                    if(isOrphan())
                        return;
                }
            }

            if(withBody)
            {
                logData(_request.body, true, "?");
                if(isOrphan())
                    return;
                httpRequest->data(std::move(_request.body), true);
                if(isOrphan())
                    return;
            }

            log([&]{ return "? done"; });
            if(isOrphan())
                return;
            httpRequest->done();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Io::started()
    {
        return _started;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::log(auto&& f)
    {
        if(_connection)
            if(const api::agent::log::Stream<>::Opposite& logStream = _connection->logStream())
                logStream->content(f());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::logHeaders(const primitives::List<api::http::Header>& headers, bool done, std::string prefix)
    {
        for(const api::http::Header& header : headers)
        {
            header.key.visit([&](const auto& k)
            {
                if constexpr(!std::is_same_v<const String&, decltype(k)>)
                    log([&]{ return prefix + " " + std::string{enumSupport::toString(k).value_or("?")} + ": " + header.value; });
                else
                    log([&]{ return prefix + " " + k + ": " + header.value; });
            });
        }
        if(done)
            log([&]{ return prefix + " headers done"; });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::logData(const Bytes& data, bool done, std::string prefix)
    {
        log([&]{ return prefix + " data " + std::to_string(data.size()) + " " + (done ? "done" : "...");});
        log([&]{ return prefix + " data content: [" + data.toString() + "]";});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        char lc(char c)
        {
            if('A' <= c && c <= 'Z')
                c = c - 'A' + 'a';
            return c;
        }

        bool containsIC(const auto& strHaystack, const auto& strNeedle)
        {
            return strHaystack.end() != std::search(
                                            std::begin(strHaystack), std::end(strHaystack),
                                            std::begin(strNeedle),   std::end(strNeedle),
                                            [](char a, char b){ return lc(a) == lc(b); }
            );
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Io::onResponseDone()
    {
        bool connectionClose = api::http::firstLine::Version::HTTP_1_1 != _responseAccumuler.version;
        primitives::List<String> setCookieValues;
        for(const api::http::Header& header : _responseAccumuler.headers)
        {
            switch(header.key.getOr(api::http::header::KeyRecognized::null))
            {
            case api::http::header::KeyRecognized::Set_Cookie:
                if(_cookies)
                    setCookieValues.emplace_back(header.value);
                break;
            case api::http::header::KeyRecognized::Connection:
                if(containsIC(header.value, "close"))
                    connectionClose = true;
                if(containsIC(header.value, "keep-alive"))
                    connectionClose = false;
                break;
            default:
                break;
            }
        }

        if(_cookies && !setCookieValues.empty())
        {
            dbgAssert(_site);
            const site::Endpoint& siteEndpoint = _site->endpoint();
            _cookies->fromResponse(siteEndpoint._host, _uriParsed._path, std::move(setCookieValues));
        }

        _sol.flush();
        if(!_responsePromise.resolved())
        {
            _responsePromise.resolveValue(std::move(_responseAccumuler));
        }

        dbgAssert(_connection);
        if(connectionClose)
            _connection->ioWantClose(rcptr());
        else
            _connection->ioDone(rcptr());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Io::isOrphan() const
    {
        return !_agent || !_site || !_connection;
    };
}
