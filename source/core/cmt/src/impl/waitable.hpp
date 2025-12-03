// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/cmt/details/wwLink.hpp>

namespace dci::cmt::impl
{
    using namespace dci::cmt::details;

    class Waitable
    {
        Waitable(const Waitable&) = delete;
        void operator=(const Waitable&) = delete;

    public:
        Waitable(
                bool (* canAcquire)(const Waitable*),
                bool (* tryAcquire)(Waitable*));
        ~Waitable();
        static void tryDestruction(Waitable*);

        void wait();

    public:
        bool canAcquire() const;
        bool tryAcquire();

        void beginAcquire(WWLink* link);
        void endAcquire(WWLink* link);

    protected:
        void throwTaskStopIfNeed();

    protected:
        utils::IntrusiveDlist<WWLink> _links;
        std::size_t _linksAmount = 0;

    private:
        bool (* _canAcquire)(const Waitable* self) = nullptr;
        bool (* _tryAcquire)(Waitable* self) = nullptr;

    };
}
