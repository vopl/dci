// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <cstdint>

namespace dci::sbs
{
    class Subscription;
}

namespace dci::sbs::impl
{
    class Box;
    class Owner;

    class Subscription final
    {
    public:
        using Activator = void (*)(sbs::Subscription*, void*, std::uint_fast8_t);

        Subscription(Activator activator, Owner* owner);
        ~Subscription();

        void removeSelf();
        void activate(void* context, std::uint_fast8_t flags, bool deletionRequested);

    private:
        friend class Box;
        friend class Owner;

        Activator _activator = nullptr;

        uint32_t        _activationDepth = 0;
        bool            _deletionRequested = false;
        bool            _deletionActivated = false;

        Owner*          _owner      = nullptr;
        Subscription*   _prevInOwner= nullptr;
        Subscription*   _nextInOwner= nullptr;

        Box*            _box        = nullptr;
        Subscription*   _prevInBox  = nullptr;
        Subscription*   _nextInBox  = nullptr;
    };
}
