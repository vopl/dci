// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::service::aup_legacy_since_2025_04::consumer::base
{
    class Downloader;
    class Remote;

    namespace remote
    {
        using Api = api_legacy_since_2025_04::Supplier<>;
        using Ptr = std::unique_ptr<Remote>;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Remote
    {
    public:
        Remote(remote::Api&& api);
        ~Remote();

        sbs::Owner& sol();
        const remote::Api& api();

        void uninvolved();
        void involve(Downloader* d);
        void uninvolve(Downloader* d);

    private:
        sbs::Owner          _sbsOwner;
        remote::Api         _api;
        Set<Downloader*>    _downloaders;
    };
}
