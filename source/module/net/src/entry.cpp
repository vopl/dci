/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "net.hpp"
#include "host.hpp"
#include "utils/makeError.hpp"

#include "net-stiac-support.hpp"

namespace dci::module::net
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

                pushServiceId<dci::idl::gen::net::Host>();
            }
        } manifest_;

        struct Entry
            : public dci::host::module::Entry
        {
            dci::idl::WeakInterface _hostWeakHolder;

            const Manifest& manifest() override
            {
                return manifest_;
            }

            dci::cmt::Future<dci::idl::Interface> createService(dci::idl::ILid ilid) override
            {
                if(dci::idl::gen::net::Host<>::lid() == ilid)
                {
                    dci::idl::Interface hostHolder = _hostWeakHolder;

                    if(!hostHolder)
                    {
                        hostHolder = tryCreateService<Host>(ilid);
                        _hostWeakHolder = hostHolder;
                    }

                    return dci::cmt::readyFuture(std::move(hostHolder));
                }

                return dci::host::module::Entry::createService(ilid);
            }
        } entry_;
    }
}

extern "C"
{
    DCI_INTEGRATION_APIDECL_EXPORT dci::host::module::Entry* dciModuleEntry = &dci::module::net::entry_;
}
