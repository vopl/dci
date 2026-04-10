/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "factory.hpp"

namespace dci::module::ppn::service::slave
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Factory::Factory(host::Manager* hostManager, const Rules& rules)
        : idl::gen::ppn::service::slave::Factory<>::Opposite{idl::interface::Initializer{}}
        , _hostManager{hostManager}
        , _rules{rules}
    {
        methods()->getInstance() += serviceSol() * [this](idl::ILid ilid)
        {
            bool allowByMask{};
            bool denyByMask{};
            bool allowConcrete{};
            bool denyConcrete{};
            for(const slave::Rule& rule : _rules)
            {
                if(rule._all)
                {
                    if(rule._allow)
                    {
                        allowByMask = true;
                        denyByMask = false;
                    }

                    if(rule._deny)
                    {
                        allowByMask = false;
                        denyByMask = true;
                    }
                }

                if(rule._concrete.contains(ilid))
                {
                    if(rule._allow)
                    {
                        allowConcrete = true;
                        denyConcrete = false;
                    }

                    if(rule._deny)
                    {
                        allowConcrete = false;
                        denyConcrete = true;
                    }
                }
            }

            if(denyConcrete)
            {
                return cmt::readyFuture<idl::Interface>(exception::buildInstance<api::error::Forbidden>());
            }
            if(!allowConcrete)
            {
                if(denyByMask)
                {
                    return cmt::readyFuture<idl::Interface>(exception::buildInstance<api::error::Forbidden>());
                }
                if(!allowByMask)
                {
                    return cmt::readyFuture<idl::Interface>(exception::buildInstance<api::error::Forbidden>());
                }
            }

            return _hostManager->createService(ilid);
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Factory::~Factory()
    {
        serviceSol().flush();
    }
}
