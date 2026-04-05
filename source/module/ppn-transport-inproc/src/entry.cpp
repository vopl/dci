/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "connector.hpp"
#include "acceptor.hpp"
#include "channelBridge/pumper.hpp"

namespace dci::module::ppn::transport::inproc
{
    namespace
    {
        struct Manifest
            : public dci::host::module::Manifest
        {
            Manifest()
            {
                _valid = true;
                _name = dciModuleName;
                _mainBinary = dciUnitTargetFile;

                pushServiceId<api::Connector>();
                pushServiceId<api::Acceptor>();
            }
        } manifest_;


        struct Entry
            : public dci::host::module::Entry
        {
            const Manifest& manifest() override
            {
                return manifest_;
            }

            bool start(dci::host::Manager* manager) override
            {
                if(!dci::host::module::Entry::start(manager))
                {
                    return false;
                }

                dbgAssert(!channelBridge::g_pumperPtr);
                if(!channelBridge::g_pumperPtr)
                {
                    channelBridge::g_pumperPtr = new channelBridge::Pumper;
                }

                return true;
            }

            bool stop() override
            {
                dbgAssert(channelBridge::g_pumperPtr);
                if(channelBridge::g_pumperPtr)
                {
                    delete channelBridge::g_pumperPtr;
                    channelBridge::g_pumperPtr = nullptr;
                }

                return dci::host::module::Entry::stop();
            }

            cmt::Future<idl::Interface> createService(idl::ILid ilid) override
            {
                if(auto s = tryCreateService<Connector>(ilid)) return cmt::readyFuture(s);
                if(auto s = tryCreateService<Acceptor>(ilid)) return cmt::readyFuture(s);

                return dci::host::module::Entry::createService(ilid);
            }
        } entry_;
    }
}

extern "C"
{
    DCI_INTEGRATION_APIDECL_EXPORT dci::host::module::Entry* dciModuleEntry = &dci::module::ppn::transport::inproc::entry_;
}
