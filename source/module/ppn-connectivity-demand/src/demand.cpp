/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
