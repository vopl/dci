// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::service::aup_legacy_since_2025_04::consumer::base
{
    class Downloader;

    class Quota
    {
    public:
        Quota(std::size_t slots_=1);
        ~Quota();

        void clear();

        void setSlots(std::size_t v);

        void ready(Downloader* downloader);
        void done(Downloader* downloader);

    private:
        void update();

    private:
        std::size_t _slots;

    private:
        struct QueueCmp
        {
            bool operator()(Downloader*a, Downloader*b) const;
        };

        using Queue = std::set<Downloader*, QueueCmp>;
        Queue _ready;

        using Set = std::set<Downloader*>;
        Set _inprogress;
    };
}
