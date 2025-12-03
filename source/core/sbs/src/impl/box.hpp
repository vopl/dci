// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "owner.hpp"
#include "subscription.hpp"
#include <dci/utils/intrusiveDlist.hpp>
#include <cstdint>

namespace dci::sbs::impl
{
    class Box final
    {
    public:
        Box();
        ~Box();

        bool empty() const;
        void push(Subscription* subscription);
        Subscription* remove(Subscription* subscription);
        void removeAndDelete(Subscription* subscription);
        void removeAndDeleteAll();
        void activate(void* context, std::uint_fast8_t flags);

    private:
        Subscription* _first = nullptr;
        Subscription* _last = nullptr;

        struct Enumerator
            : utils::IntrusiveDlistElement<Enumerator>
        {
            Enumerator(Subscription* nextSubscription, Subscription* lastSubscription)
                : _nextSubscription{nextSubscription}
                , _lastSubscription{lastSubscription}
            {}

            Subscription* _nextSubscription;
            Subscription* _lastSubscription;
        };

        utils::IntrusiveDlist<Enumerator> _enumerators;
    };
}
