// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "logger.hpp"

namespace std
{
    using namespace dci::module::ppn::node;

    inline std::ostream& operator<<(std::ostream& o, const api::link::Remote<>& r)
    {
        (void) r;
        return o;//<<"r";
    }

    inline std::ostream& operator<<(std::ostream& o, const transport::Address& a)
    {
        return o<<a.value;
    }

    inline std::ostream& operator<<(std::ostream& o, const api::link::Id& id)
    {
        o<<dci::utils::b2h(id.data(), id.size());
        return o;
    }

    inline std::ostream& operator<<(std::ostream& o, const ExceptionPtr& e)
    {
        return o<<dci::exception::toString(e);
    }
}

namespace dci::module::ppn::node
{
    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        std::string addr2String(const transport::Address& a1, const transport::Address& a2)
        {
            if(!a1.value.empty() && !a2.value.empty())
            {
                if(a1 == a2)
                {
                    return a1.value;
                }

                return a1.value + " (" + a2.value + ")";
            }

            return a1.value + a2.value;
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        bool parseBool(const String& param)
        {
            static const std::regex t("^(t|true|on|enable|allow|1|)$", std::regex_constants::icase | std::regex::optimize);
            return std::regex_match(param, t);
        }

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Logger::Logger()
        : api::Logger<>::Opposite(idl::interface::Initializer{})
    {
        //link
        {
            api::link::Feature<>::Opposite op = *this;

            //in setup(feature::Service);
            op->setup() += serviceSol() * [this](api::link::feature::Service<> srv)
            {
                //out joinedByConnect(Remote);
                srv->joinedByConnect() += serviceSol() * [this](const api::link::Id& id, const api::link::Remote<>& r)
                {
                    if(test("link", "remote", "joinedByConnect"))
                    {
                        r->remoteAddress().then() += serviceSol() * [id](auto in)
                        {
                            if(in.resolvedValue())
                            {
                                LOGI("remote joinedByConnect"<<": "<<id<<", address: "<<in.detachValue());
                            }
                            else if(in.resolvedException())
                            {
                                LOGI("remote joinedByConnect"<<": "<<id<<", address obtaining failed: "<<in.exception());
                            }
                            else// if(in.resolvedCancel())
                            {
                                LOGI("remote joinedByConnect"<<": "<<id<<", address obtaining cancelled");
                            }
                        };
                    }

                    r->closed() += serviceSol() * [id,this]()
                    {
                        if(test("link", "remote", "closed")) LOGI("remote closed: "<<id);
                    };
                    r->failed() += serviceSol() * [id,this](const ExceptionPtr& e)
                    {
                        if(test("link", "remote", "failed")) LOGI("remote failed: "<<id<<", "<<e);
                    };
                };

                //out joinedByAccept(Remote);
                srv->joinedByAccept() += serviceSol() * [this](const api::link::Id& id, const api::link::Remote<>& r)
                {
                    if(test("link", "remote", "joinedByAccept"))
                    {
                        r->remoteAddress().then() += serviceSol() * [id](auto in)
                        {
                            if(in.resolvedValue())
                            {
                                LOGI("remote joinedByAccept"<<": "<<id<<", address: "<<in.detachValue());
                            }
                            else if(in.resolvedException())
                            {
                                LOGI("remote joinedByAccept"<<": "<<id<<", address obtaining failed: "<<in.exception());
                            }
                            else// if(in.resolvedCancel())
                            {
                                LOGI("remote joinedByAccept"<<": "<<id<<", address obtaining cancelled");
                            }
                        };
                    }

                    r->closed() += serviceSol() * [id,this]()
                    {
                        if(test("link", "remote", "closed")) LOGI("remote closed: "<<id);
                    };
                    r->failed() += serviceSol() * [id,this](const ExceptionPtr& e)
                    {
                        if(test("link", "remote", "failed")) LOGI("remote failed: "<<id<<", "<<e);
                    };
                };

                if(test("link", "local", "id"))
                {
                    srv->id().then() += serviceSol() * [](auto in)
                    {
                        if(in.resolvedValue())
                        {
                            LOGI("local link id: "<<in.value());
                        }
                        else if(in.resolvedException())
                        {
                            LOGW("local link id obtaining failed: "<<in.exception());
                        }
                        else// if(in.resolvedCancel())
                        {
                            LOGW("local link id obtaining cancelled");
                        }
                    };
                }
            };
        }

        //node
        {
            api::Feature<>::Opposite op = *this;

            //in setup(feature::Service);
            op->setup() += serviceSol() * [this](api::feature::Service<> srv)
            {
                //out start();
                srv->start() += serviceSol() * [this]()
                {
                    if(test("start")) LOGI("start");
                };

                //out stop();
                srv->stop() += serviceSol() * [this]()
                {
                    if(test("stop")) LOGI("stop");
                };

                //out failed(exception);
                srv->failed() += serviceSol() * [this](const ExceptionPtr& e)
                {
                    if(test("failed")) LOGI("failed: "<<e);
                };

                {
                    api::feature::RemoteAddressSpace<> ras = srv;

                    //out discovered(link::Id, transport::Address);
                    ras->discovered() += serviceSol() * [this](const api::link::Id& id, const transport::Address& a)
                    {
                        if(test("ras", "discovered")) LOGI("discovered: "<<id<<", "<<a);
                    };
                }

                {
                    api::feature::LocalAddressSpace<> las = srv;

                    //out declared(transport::Address);
                    las->declared() += serviceSol() * [this](const transport::Address& a)
                    {
                        if(test("las", "declared")) LOGI("declared: "<<a);
                    };

                    //out undeclared(transport::Address);
                    las->undeclared() += serviceSol() * [this](const transport::Address& a)
                    {
                        if(test("las", "undeclared")) LOGI("undeclared: "<<a);
                    };
                }

                {
                    api::feature::Connectors<> csrv = srv;

                    //out connectorStarted(transport::Address);
                    csrv->connectorStarted() += serviceSol() * [this](const transport::Address& a)
                    {
                        if(test("connector", "started")) LOGI("connectorStarted: "<<a);
                    };

                    //out connectorStopped(transport::Address);
                    csrv->connectorStopped() += serviceSol() * [this](const transport::Address& a)
                    {
                        if(test("connector", "stopped")) LOGI("connectorStopped: "<<a);
                    };

                    //out newSession(link::Id, transport::Address, CSession);
                    csrv->newSession() += serviceSol() * [this](const api::link::Id& id, const transport::Address& a, api::feature::CSession<> s) mutable
                    {
                        static uint64 cidGen = 0;
                        uint64 cid = ++cidGen;

                        if(test("connector", "session", "new")) LOGI("new csession "<<cid<<", id: "<<id<<", address: "<<a);

                        //out connected();
                        s->connected() += serviceSol() * [cid,this]
                        {
                            if(test("connector", "session", "connected")) LOGI("csession "<<cid<<" connected");
                        };

                        //out idSpecified(link::Id);
                        s->idSpecified() += serviceSol() * [cid,this](const api::link::Id& id)
                        {
                            if(test("connector", "session", "idSpecified")) LOGI("csession "<<cid<<" idSpecified: "<<id);
                        };

                        //out failed(exception);
                        s->failed() += serviceSol() * [cid,this](ExceptionPtr e)
                        {
                            if(test("connector", "session", "failed")) LOGI("csession "<<cid<<" failed: "<<e);
                        };

                        //out joined();
                        s->joined() += serviceSol() * [cid,this](const api::link::Remote<>&)
                        {
                            if(test("connector", "session", "joined")) LOGI("csession "<<cid<<" joined");
                        };

                        //out closed();
                        s->closed() += serviceSol() * [cid,this]
                        {
                            if(test("connector", "session", "closed")) LOGI("csession "<<cid<<" closed");
                        };
                    };
                }

                {
                    api::feature::Acceptors<> asrv = srv;

                    //out acceptorStarted(transport::Address);
                    asrv->acceptorStarted() += serviceSol() * [this](const transport::Address& a1, const transport::Address& a2)
                    {
                        if(test("acceptor", "started")) LOGI("acceptorStarted: "<<addr2String(a1, a2));
                    };

                    //out acceptorStopped(transport::Address);
                    asrv->acceptorStopped() += serviceSol() * [this](const transport::Address& a1, const transport::Address& a2)
                    {
                        if(test("acceptor", "stopped")) LOGI("acceptorStopped: "<<addr2String(a1, a2));
                    };

                    //out acceptorFailed(transport::Address, exception);
                    asrv->acceptorFailed() += serviceSol() * [this](const transport::Address& a1, const transport::Address& a2, const ExceptionPtr& e)
                    {
                        if(test("acceptor", "failed")) LOGI("acceptorFailed: "<<addr2String(a1, a2)<<", "<<e);
                    };

                    //out newSession(transport::Address, ASession);
                    asrv->newSession() += serviceSol() * [this](api::feature::ASession<> s) mutable
                    {
                        static uint64 cidGen = 0;
                        uint64 cid = ++cidGen;

                        if(test("acceptor", "session", "new"))
                        {
                            s->address().then() += serviceSol() * [cid](auto in)
                            {
                                if(in.resolvedValue())
                                {
                                    LOGI("new asession "<<cid<<", address: "<<in.detachValue());
                                }
                                else if(in.resolvedException())
                                {
                                    LOGW("new asession "<<cid<<", address obtaining failed: "<<in.exception());
                                }
                                else// if(in.resolvedCancel())
                                {
                                    LOGW("new asession "<<cid<<", address obtaining cancelled");
                                }
                            };
                        }

                        //out idSpecified(link::Id);
                        s->idSpecified() += serviceSol() * [cid,this](const api::link::Id& id)
                        {
                            if(test("acceptor", "session", "idSpecified")) LOGI("asession "<<cid<<" idSpecified: "<<id);
                        };

                        //out failed(exception);
                        s->failed() += serviceSol() * [cid,this](ExceptionPtr e)
                        {
                            if(test("acceptor", "session", "failed")) LOGI("asession "<<cid<<" failed: "<<e);
                        };

                        //out joined();
                        s->joined() += serviceSol() * [cid,this](const api::link::Remote<>&)
                        {
                            if(test("acceptor", "session", "joined")) LOGI("asession "<<cid<<" joined");
                        };

                        //out closed();
                        s->closed() += serviceSol() * [cid,this]
                        {
                            if(test("acceptor", "session", "closed")) LOGI("asession "<<cid<<" closed");
                        };
                    };
                }
            };
        }

        //in configure(Config) -> void;
        methods()->configure() += serviceSol() * [this](idl::gen::Config&& config)
        {
            _config.clear();
            std::function<void(const String&, const config::ptree&)> traverse = [&,this](const String& prefix, const config::ptree& pt)
            {
                for(const auto&[k, v] : pt)
                {
                    const String& akey = prefix.empty() ? k : prefix+"."+k;
                    _config[utils::fnv1a(akey)] = parseBool(v.data());
                    traverse(akey, v);
                }
            };
            traverse({}, config::cnvt(std::move(config)));

            return cmt::readyFuture(None{});
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Logger::~Logger()
    {
        serviceSol().flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Logger::test(const auto& key)
    {
        auto iter = _config.find(utils::fnv1a(key));
        if(_config.end() == iter)
        {
            return true;
        }

        return iter->second;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Logger::test(const auto& prefix, const auto& head, const auto&... tail)
    {
        if(!test(prefix))
        {
            return false;
        }

        char prefix2[sizeof(prefix) + sizeof(head)];

        std::copy(&prefix[0], &prefix[sizeof(prefix)-1], &prefix2[0]);
        prefix2[sizeof(prefix)-1] = '.';
        std::copy(&head[0], &head[sizeof(head)], &prefix2[sizeof(prefix)]);

        return test(prefix2, tail...);
    }
}
