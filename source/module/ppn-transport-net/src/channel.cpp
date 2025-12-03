// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "channel.hpp"
#include "endpoint2Address.hpp"

namespace dci::module::ppn::transport::net
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::Channel(apit::Address&& originalRemoteAddress, idl::gen::net::stream::Channel<>&& netStreamChannel)
        : apit::Channel<>::Opposite(idl::interface::Initializer{})
        , _originalRemoteAddress(std::move(originalRemoteAddress))
        , _netStreamChannel(std::move(netStreamChannel))
    {
        methods()->localAddress() += this * [this]()
        {
            return _netStreamChannel->localEndpoint().chain<apit::Address>(*this, [](auto in, auto& out)
            {
                if(in.resolvedValue())
                {
                    out.resolveValue(endpoint2Address(in.detachValue()));
                }
                else if(in.resolvedException())
                {
                    out.resolveException(in.detachException());
                }
                else
                {
                    out.resolveCancel();
                }
            });
        };

        methods()->remoteAddress() += this * [this]()
        {
            return _netStreamChannel->remoteEndpoint().chain<apit::Address>(*this, [](auto in, auto& out)
            {
                if(in.resolvedValue())
                {
                    out.resolveValue(endpoint2Address(in.detachValue()));
                }
                else if(in.resolvedException())
                {
                    out.resolveException(in.detachException());
                }
                else
                {
                    out.resolveCancel();
                }
            });
        };

        methods()->originalRemoteAddress() += this * [this]()
        {
            return cmt::readyFuture(_originalRemoteAddress);
        };

        methods()->unlockInput() += this * [this]() -> void
        {
            return _netStreamChannel->startReceive();
        };

        methods()->lockInput() += this * [this]() -> void
        {
             return _netStreamChannel->stopReceive();
        };

        _netStreamChannel->failed() += this * [this](auto&& e)
        {
            methods()->failed(std::forward<decltype(e)>(e));
        };

        methods()->close() += this * [this]() -> void
        {
            _netStreamChannel->close();
        };

        _netStreamChannel->closed() += this * [this]()
        {
            methods()->closed();
        };

        _netStreamChannel->received() += this * [this](auto&& data)
        {
            methods()->input(std::forward<decltype(data)>(data));
        };

        methods()->output() += this * [this](auto&& data)
        {
            _netStreamChannel->send(std::forward<decltype(data)>(data));
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::~Channel()
    {
        flush();
    }
}
