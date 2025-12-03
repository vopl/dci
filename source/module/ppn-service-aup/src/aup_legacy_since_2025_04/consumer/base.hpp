// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
