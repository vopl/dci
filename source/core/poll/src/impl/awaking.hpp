// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "awaker.hpp"
#include <dci/utils/intrusiveDlist.hpp>
#include <mutex>

namespace dci::poll::impl
{
    class Awaking
    {
    public:
        Awaking();
        ~Awaking();

        void install(Awaker* awaker);
        void uninstall(Awaker* awaker);

        void ready(Awaker* awaker);
        void unready(Awaker* awaker);

    public:
        bool woken();
        bool hasPayload() const;

    private:
        mutable std::recursive_mutex _mtx;
        utils::IntrusiveDlist<Awaker, TagForAll>    _awakers;
        std::size_t                                 _keepLoop{};
        utils::IntrusiveDlist<Awaker, TagForReady>  _awakersReady;
    };
}
