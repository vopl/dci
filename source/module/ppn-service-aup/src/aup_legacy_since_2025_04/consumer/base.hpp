/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "pch.hpp"
#include "base/remote.hpp"
#include "base/downloader.hpp"
#include "base/quota.hpp"
#include "base/recvBuffer.hpp"

namespace dci::module::ppn::service
{
    class Aup;
}

namespace dci::module::ppn::service::aup_legacy_since_2025_04::consumer
{
    class Base
    {
    public:
        Base(std::string_view name, base::Quota* quota);
        virtual ~Base();

        base::Remote* involve(base::remote::Api&& remoteApi);

    protected:
        bool addIncomplete(const Oid& oid, int priority);
        virtual bool onComplete(const Oid& oid, base::RecvBuffer& recvBuffer) = 0;

        friend class base::Downloader;
        base::Quota& quota();
        bool ready(base::Downloader* d, base::RecvBuffer& recvBuffer);
        void done(base::Downloader* d);

    protected:
        const std::string_view                  _name;
        Map<base::Remote*, base::remote::Ptr>   _remotes;
        Map<Oid, base::Downloader>              _downloaders;
        base::Quota *                           _quota;
        sbs::Owner                              _sbsOwner;
    };
}
