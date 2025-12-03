// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "demand.hpp"

namespace dci::module::ppn::connectivity
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Demand::Demand()
        : idl::gen::ppn::connectivity::Demand<>::Opposite(idl::interface::Initializer())
    {
        {
            node::Feature<>::Opposite op = *this;

            op->setup() += serviceSol() * [this](node::feature::Service<> srv)
            {
                srv->start() += serviceSol() * [this, srv]() mutable
                {
                    _started = true;
                    _registry.start();
                };

                srv->stop() += serviceSol() * [this]
                {
                    _started = false;
                    _registry.stop();
                };

                srv->registerAgentProvider(api::Registry<>::lid(), *this);
            };
        }

        {
            idl::gen::Configurable<>::Opposite op = *this;

            op->configure() += serviceSol() * [this](dci::idl::gen::Config&& config)
            {
                auto c = config::cnvt(std::move(config));

                _registry.setIntensity(std::atof(c.get("intensity", "10").data()));

                return cmt::readyFuture(None{});
            };
        }

        methods()->getAgent() += serviceSol() * [this](idl::ILid ilid)
        {
            if(api::Registry<>::lid() == ilid)
            {
                return cmt::readyFuture<idl::Interface>(idl::Interface{_registry});
            }

            dbgWarn("crazy node?");
            return cmt::readyFuture<idl::Interface>(exception::buildInstance<api::Error>("bad agent ilid requested"));
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Demand::~Demand()
    {
        _started = false;

        serviceSol().flush();
    }
}
