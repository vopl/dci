// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/poll/timer.hpp>
#include <dci/sbs.hpp>
#include <dci/aup/catalog/object.hpp>
#include <dci/aup/storage.hpp>
#include <filesystem>

namespace dci::aup::instance
{
    class Importer
    {
    public:
        Importer();
        ~Importer();

        void setup(const std::filesystem::path& dir);
        void start();
        void stop();

    public:
        sbs::Signal<void> emitStart();
        sbs::Signal<void, impl::Catalog*, impl::Storage*> emitData();
        sbs::Signal<void> emitFinish();

    private:
        void onTicker();
        bool tryImport(const std::filesystem::path& path);

    private:
        std::filesystem::path   _dir;
        bool                    _started{false};

        poll::Timer             _ticker
        {
            std::chrono::seconds{1},
            true,
            [this](){ onTicker(); }
        };

        using SomeFound = std::map<std::filesystem::path /*_someFoundPath*/, std::chrono::steady_clock::time_point /*_someFoundMoment*/>;
        SomeFound _someFound;

    private:
        sbs::Wire<void>                                 _emitStart;
        sbs::Wire<void, impl::Catalog*, impl::Storage*> _emitData;
        sbs::Wire<void>                                 _emitFinish;
    };
}
