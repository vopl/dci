/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "../api.hpp"
#include "manifest.hpp"
#include "serviceBase.hpp"
#include <dci/cmt/future.hpp>
#include <dci/cmt/promise.hpp>
#include <dci/idl/interface.hpp>
#include <dci/idl/iLid.hpp>
#include <string>

namespace dci::host
{
    class Manager;
}

namespace dci::host::module
{
    class StopLocker;

    struct API_DCI_HOST Entry
    {
        Entry();
        virtual ~Entry();

        virtual const Manifest& manifest() = 0;

        virtual bool load();
        virtual bool unload();

        virtual bool start(Manager* manager);
        virtual cmt::Future<> stopRequest();
        virtual bool stop();

        virtual cmt::Future<idl::Interface> createService(idl::ILid ilid);

        Manager* manager() const;
        StopLocker stopLocker();

    protected:
        template <class Srv>
        static idl::Interface tryCreateService(idl::ILid ilid, auto&&... args) requires std::is_base_of_v<ServiceBase<Srv>, Srv>;

    private:
        friend class StopLocker;
        void stopLockCounterInc();
        void stopLockCounterDec();

    private:
        Manager *       _manager = nullptr;
        std::size_t     _stopLockCounter {};
        cmt::Promise<>  _stopLock;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Srv>
    idl::Interface Entry::tryCreateService(idl::ILid ilid, auto&&... args) requires std::is_base_of_v<ServiceBase<Srv>, Srv>
    {
        if(Srv::Opposite::lid() != ilid)
        {
            return idl::Interface();
        }

        Srv* srv = new Srv{std::forward<decltype(args)>(args)...};
        srv->involvedChanged() += srv->serviceSol() * [srv](bool v)
        {
            if(!v)
            {
                delete srv;
            }
        };

        return idl::Interface(srv->opposite());
    }
}
