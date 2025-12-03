// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "connector.hpp"
#include "acceptor.hpp"
#include "channelBridge.hpp"

namespace dci::module::ppn::transport::inproc
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Connector::Connector()
        : apit::inproc::Connector<>::Opposite(idl::interface::Initializer())
    {
        //in address() -> transport::Address;
        methods()->address() += serviceSol() * []
        {
            return cmt::readyFuture(apit::Address{String{"inproc://"}});
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

        //in connect(Address) -> Channel;
        methods()->connect() += serviceSol() * [](const apit::Address& address) -> cmt::Future<apit::Channel<>>
        {
            if(!utils::uri::valid<utils::uri::Inproc<>>(address.value))
            {
                return cmt::readyFuture<apit::Channel<>>(exception::buildInstance<api::BadAddress>());
            }

            Acceptor* acceptor = Acceptor::findAcceptor(address.value);
            if(!acceptor)
            {
                return cmt::readyFuture<apit::Channel<>>(exception::buildInstance<api::ConnectionRefused>());
            }

            auto pair = ChannelBridge::allocate(address);

            (*acceptor)->accepted(pair.first);
            return cmt::readyFuture(pair.second);
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Connector::~Connector()
    {
        serviceSol().flush();
    }
}
