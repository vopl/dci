/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/host/manager.hpp>
#include "impl/manager.hpp"

#include "idl-host.hpp"

namespace dci::host
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    int Manager::executeTest(const std::vector<std::string>& argv, TestStage stage)
    {
        return impl::Manager::executeTest(argv, stage, nullptr);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const module::Manifest& Manager::moduleManifest(const std::string& mainBinaryFullPath)
    {
        return impl::Manager::moduleManifest(mainBinaryFullPath);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Manager::Manager()
        : himpl::FaceLayout<Manager, impl::Manager>()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Manager::~Manager()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Manager::run()
    {
        return impl().run();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Manager::stop()
    {
        return impl().stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Manager::startModules(std::set<std::string> &&modules, std::set<std::string> &&services)
    {
        return impl().startModules(std::move(modules), std::move(services));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<int> Manager::runTest(const std::vector<std::string>& argv, TestStage stage)
    {
        return impl().runTest(argv, stage);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<> Manager::runDaemon(const std::vector<std::string>& argv)
    {
        return impl().runDaemon(argv);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<> Manager::runDaemons(const std::vector<std::string>& argv)
    {
        return impl().runDaemons(argv);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::optional<idl::ILid> Manager::resolveAlias(const std::string& alias)
    {
        return impl().resolveAlias(alias);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<idl::Interface> Manager::createService(const idl::IId& iid)
    {
        return impl().createService(idl::ILid {idl::contract::lidRegistry.get(iid._cid), iid._side});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<idl::Interface> Manager::createService(idl::ILid ilid)
    {
        return impl().createService(ilid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<idl::Interface> Manager::createService(const std::string& alias)
    {
        return impl().createService(alias);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<idl::Interface> Manager::getDaemonService(const std::string& name)
    {
        return impl().getDaemonService(name);
    }

}
