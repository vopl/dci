// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "cookies.hpp"
#include "cookies/parsers.hpp"
#include "cookies/path.hpp"
#include "cookies/domain.hpp"

namespace x3 = boost::spirit::x3;

namespace dci::module::www::http::client
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cookies::Cookies()
        : api::http::client::Cookies<>::Opposite{idl::interface::Initializer{}}
        , _store{*this, serviceSol()}
    {
        // in fromResponse(
        //     string domain,
        //     string path,
        //     list<string>) -> none;
        methods()->fromResponse() += serviceSol() * [this](String&& domain, String&& path, const List<String>& setCookieHeaders)
        {
            cookies::path::directoryFromResource(path);

            bool hostOnly{};
            switch(cookies::domain::canonicalize(domain))
            {
            case dci::utils::dns::CanonicalizeResult::ip:
                hostOnly = true;
                break;
            case dci::utils::dns::CanonicalizeResult::unneeded:
            case dci::utils::dns::CanonicalizeResult::ok:
                break;
            case dci::utils::dns::CanonicalizeResult::badInput:
            default:
                LOGD("failed to canonicalize domain: [" << domain << "]");
                return;
            }

            uint64 now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::utc_clock::now().time_since_epoch()).count();
            _store.expire(now, false);

            for(const String& setCookieHeader: setCookieHeaders)
            {
                auto begin = setCookieHeader.begin();
                const auto end = setCookieHeader.end();
                api::http::client::cookies::Entry entry;

                entry.creationTime = now;
                entry.lastAccessTime = entry.creationTime;
                entry.expiryTime = std::numeric_limits<decltype(entry.expiryTime)>::max();

                bool parseResult = x3::parse(
                    begin,
                    setCookieHeader.end(),
                    x3::with<api::http::client::cookies::Entry>(entry)[cookies::parsers::setCookieString]);

                if(!parseResult || begin != end)
                {
                    LOGD("failed to parse Set-Cookie header: [" << setCookieHeader << "]");
                    continue;
                }

                if(entry.id.domain.empty())
                {
                    entry.id.hostOnly = true;
                    entry.id.domain = domain;
                }
                else
                {
                    switch(cookies::domain::canonicalize(entry.id.domain))
                    {
                    case dci::utils::dns::CanonicalizeResult::ip:
                        entry.id.hostOnly = true;
                        break;
                    case dci::utils::dns::CanonicalizeResult::unneeded:
                    case dci::utils::dns::CanonicalizeResult::ok:
                        break;
                    case dci::utils::dns::CanonicalizeResult::badInput:
                    default:
                        LOGD("failed to canonicalize domain: [" << entry.id.domain << "]");
                        continue;
                    }

                    if(hostOnly)
                    {
                        if(!entry.id.hostOnly)
                            continue;

                        if(entry.id.domain != domain)
                        {
                            //LOGD("crossdomain attempt: [" << entry.domain << "] <- [" << domain << "]");
                            continue;
                        }
                    }
                    else
                    {
                        if(!cookies::domain::matched(domain, entry.id.domain))
                        {
                            //LOGD("crossdomain attempt: [" << entry.domain << "] <- [" << domain << "]");
                            continue;
                        }
                    }
                }

                cookies::path::canonicalize(entry.id.path, path);

                _store.set(std::move(entry));
            }
        };

        // in toRequest(
        //     string domain,
        //     string path,
        //     bool isSecure,
        //     bool isHttp) -> list<string>;
        methods()->toRequest() += serviceSol() * [this](String&& domain, String&& path, bool isSecure, bool isHttp)
        {
            cookies::path::directoryFromResource(path);

            bool hostOnly{};
            switch(cookies::domain::canonicalize(domain))
            {
            case dci::utils::dns::CanonicalizeResult::ip:
                hostOnly = true;
                break;
            case dci::utils::dns::CanonicalizeResult::unneeded:
            case dci::utils::dns::CanonicalizeResult::ok:
                break;
            case dci::utils::dns::CanonicalizeResult::badInput:
            default:
                LOGD("failed to canonicalize domain: [" << domain << "]");
                return cmt::readyFuture(List<String>{});
            }

            uint64 now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            _store.expire(now, false);

            List<api::http::client::cookies::Entry*> entries;
            _store.traverse([&](api::http::client::cookies::Entry& entry)
            {
                if(entry.httpOnly && !isHttp)
                    return;
                if(entry.secure && !isSecure)
                    return;
                if(entry.id.hostOnly)
                {
                    if(entry.id.domain != domain)
                        return;
                }
                else
                {
                    if(hostOnly)
                    {
                        if(entry.id.domain != domain)
                            return;
                    }
                    else
                    {
                        if(!cookies::domain::matched(domain, entry.id.domain))
                            return;
                    }
                }
                if(!cookies::path::matched(path, entry.id.path))
                    return;
                entries.push_back(&entry);
            });
            std::stable_sort(entries.begin(), entries.end(), [](const api::http::client::cookies::Entry* a, const api::http::client::cookies::Entry* b)
            {
                return std::tuple(-(int64_t)a->id.path.size(), a->creationTime) < std::tuple(-(int64_t)b->id.path.size(), b->creationTime);
            });

            List<String> res;
            for(api::http::client::cookies::Entry* entry : entries)
            {
                entry->lastAccessTime = now;

                if(res.empty() /*|| res.back().size() >= */)
                    res.emplace_back(String{});
                String& resStr = res.back();
                if(!resStr.empty())
                    resStr += "; ";
                resStr += entry->id.name + '=' + entry->value;
            }

            return cmt::readyFuture(std::move(res));
        };
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cookies::~Cookies()
    {
        serviceSol().flush();
    };
}
