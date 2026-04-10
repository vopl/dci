/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "api.hpp"
#include <dci/himpl.hpp>
#include <dci/host/implMetaInfo.hpp>
#include <dci/host/module/manifest.hpp>
#include <dci/host/exception.hpp>
#include <dci/idl/interface.hpp>
#include <dci/idl/iId.hpp>
#include <dci/idl/iLid.hpp>
#include <dci/cmt.hpp>
#include <dci/sbs/signal.hpp>
#include "test.hpp"

namespace dci::host
{
    class API_DCI_HOST Manager
        : public himpl::FaceLayout<Manager, impl::Manager>
    {
        Manager(const Manager&) = delete;
        void operator=(const Manager&) = delete;

    public:
        static int executeTest(const std::vector<std::string>& argv, TestStage stage);
        static const module::Manifest& moduleManifest(const std::string& mainBinaryFullPath);

    public:
        Manager();
        ~Manager();

        void run();//блокирующий
        void stop();//запрос на выход из run

        bool startModules(std::set<std::string>&& byNames, std::set<std::string>&& byServices);

        cmt::Future<int> runTest(const std::vector<std::string>& argv, TestStage stage);
        cmt::Future<> runDaemon(const std::vector<std::string>& argv);
        cmt::Future<> runDaemons(const std::vector<std::string>& argv);

        std::optional<idl::ILid> resolveAlias(const std::string& alias);

        cmt::Future<idl::Interface> createService(const idl::IId& iid);
        cmt::Future<idl::Interface> createService(idl::ILid ilid);
        cmt::Future<idl::Interface> createService(const std::string& alias);

        template <class Interface>
        cmt::Future<Interface> createService();

        cmt::Future<idl::Interface> getDaemonService(const std::string& name);

        template <class Interface>
        cmt::Future<Interface> getDaemonService(const std::string& name);
    };


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Interface>
    cmt::Future<Interface> Manager::createService()
    {
        return createService(Interface::lid()).template chain<Interface>([](auto in, cmt::Promise<Interface>& out)
        {
            if(in.resolvedException())
            {
                out.resolveException(in.detachException());
                return;
            }
            if(in.resolvedCancel())
            {
                out.resolveCancel();
                return;
            }

            Interface mdi(in.detachValue());
            if(!mdi)
            {
                out.resolveException(std::make_exception_ptr(exception::UnableToCreateService("null value from module received")));
                return;
            }

            out.resolveValue(std::move(mdi));
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Interface>
    cmt::Future<Interface> Manager::getDaemonService(const std::string& name)
    {
        return getDaemonService(name).template chain<Interface>([](auto in, cmt::Promise<Interface>& out)
        {
            if(in.resolvedException())
            {
                out.resolveException(in.detachException());
                return;
            }
            if(in.resolvedCancel())
            {
                out.resolveCancel();
                return;
            }

            Interface mdi(in.detachValue());
            if(!mdi)
            {
                out.resolveException(std::make_exception_ptr(exception::DaemonGetFail("null value from daemon received")));
                return;
            }

            out.resolveValue(std::move(mdi));
        });
    }
}
