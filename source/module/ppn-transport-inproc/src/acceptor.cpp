// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "acceptor.hpp"
#include <dci/utils/uri.hpp>

namespace dci::module::ppn::transport::inproc
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Acceptor::Acceptor()
        : apit::inproc::Acceptor<>::Opposite(idl::interface::Initializer())
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
            if(_started)
            {
                return cmt::readyFuture<None>(exception::buildInstance<api::AlreadyBound>("unable to bind after acceptor started"));
            }

            using namespace std::literals;
            if(!utils::uri::valid<utils::uri::Inproc<>>(address.value))
            {
                return cmt::readyFuture<None>(exception::buildInstance<api::BadAddress>());
            }

            _address = std::move(address);
            methods()->addressChanged(_address);
            return cmt::readyFuture(None{});
        };

        //in start();
        methods()->start() += serviceSol() * [this]
        {
            if(_started)
            {
                //already started
                return;
            }
            _started = true;

            auto p = _registry.try_emplace(_address.value, this);

            if(!p.second)
            {
                methods()->failed(_address, _address, exception::buildInstance<api::AddressAlreadyInUse>());
                return;
            }

            methods()->started(_address, _address);
        };

        //in stop();
        methods()->stop() += serviceSol() * [this]
        {
            if(_started)
            {
                _started = false;
                _registry.erase(_address.value);
                methods()->stopped(_address, _address);
            }
        };

        //out accepted(Channel);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Acceptor::~Acceptor()
    {
        serviceSol().flush();

        if(_started)
        {
            _registry.erase(_address.value);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Acceptor* Acceptor::findAcceptor(const String& address)
    {
        auto iter = _registry.find(address);
        if(_registry.end() == iter)
        {
            return nullptr;
        }

        return iter->second;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::map<String, Acceptor*> Acceptor::_registry;
}
