// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "connector.hpp"
#include "channel.hpp"
#include "address2Endpoint.hpp"

namespace dci::module::ppn::transport::net
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Connector::Connector(host::Manager* hostManager)
        : apit::net::Connector<>::Opposite(idl::interface::Initializer())
        , _hostManager(hostManager)
    {
        //in address() -> transport::Address;
        methods()->address() += serviceSol() * [this]
        {
            return cmt::readyFuture(_address);
        };

        //in cost() -> real64;
        methods()->cost() += serviceSol() * []
        {
            return cmt::readyFuture(real64{0});
        };

        //in rtt() -> real64;
        methods()->rtt() += serviceSol() * []
        {
            return cmt::readyFuture(real64{0});
        };

        //in bandwidth() -> real64;
        methods()->bandwidth() += serviceSol() * []
        {
            return cmt::readyFuture(std::numeric_limits<real64>::max());
        };

        //in bind(Address) -> void;
        methods()->bind() += serviceSol() * [this](apit::Address&& address)
        {
            auto scheme = utils::uri::scheme(address.value);
            using namespace std::literals;
            if("local"sv != scheme &&
               "tcp4"sv  != scheme &&
               "tcp6"sv  != scheme &&
               "tcp"sv   != scheme)
            {
                return cmt::readyFuture<None>(exception::buildInstance<api::BadAddress>(address.value));
            }

            return cmt::spawnv() += _tol * [this, address](cmt::Promise<None>& out)
            {
                try
                {
                    _netStreamClient.value()->bind(address2Endpoint(_netHost.value(), address));

                    _address = std::move(address);
                    methods()->addressChanged(_address);

                    out.resolveValue(None{});
                }
                catch(const cmt::task::Stop&)
                {
                    if(!out.resolved())
                    {
                        out.resolveCancel();
                    }
                }
                catch(...)
                {
                    if(!out.resolved())
                    {
                        out.resolveException(std::current_exception());
                    }
                }
            };
        };

        //in connect(Address) -> Channel;
        methods()->connect() += serviceSol() * [this](const apit::Address& address)
        {
            return cmt::spawnv() += _tol * [this, address](cmt::Promise<apit::Channel<>>& out)
            {
                cmt::task::current().stopOnResolvedCancel(out);//остановить этот воркер по отмене результата

                try
                {
                    cmt::Future<idl::gen::net::stream::Channel<>> netStreamChannelFuture = _netStreamClient.value()->connect(address2Endpoint(_netHost.value(), address));
                    poll::WaitableTimer deadline{std::chrono::seconds{2}};
                    deadline.start();

                    if(0 == cmt::waitAny(deadline.waitable(), netStreamChannelFuture.waitable()))
                    {
                        netStreamChannelFuture.resolveCancel();
                        if(!out.resolved())
                        {
                            out.resolveException(exception::buildInstance<api::ConnectionTimeout>());
                        }
                    }
                    else
                    {
                        idl::gen::net::stream::Channel<> netStreamChannel = netStreamChannelFuture.value();
                        netStreamChannel->setOption(idl::gen::net::option::NoDelay{true});

                        if(!out.resolved())
                        {
                            Channel* impl = new Channel(apit::Address{address}, std::move(netStreamChannel));
                            impl->involvedChanged() += impl * [impl](bool v)
                            {
                                if(!v)
                                {
                                    delete impl;
                                }
                            };

                            out.resolveValue(impl->opposite());
                        }
                    }
                }
                catch(const cmt::task::Stop&)
                {
                    //empty is ok
                    if(!out.resolved())
                    {
                        out.resolveCancel();
                    }
                }
                catch(...)
                {
                    if(!out.resolved())
                    {
                        out.resolveException(std::current_exception());
                    }
                }
            };
        };

        _netHost = _hostManager->createService<idl::gen::net::Host<>>();

        _netStreamClient = _netHost.chain(serviceSol(), [](cmt::Future<idl::gen::net::Host<>> in, cmt::Promise<idl::gen::net::stream::Client<>> out)
        {
            in.value()->streamClient().then() += [out=std::move(out)](auto in) mutable
            {
                out.resolveAs(in);
            };
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Connector::~Connector()
    {
        serviceSol().flush();
        _tol.stop();
    }
}
