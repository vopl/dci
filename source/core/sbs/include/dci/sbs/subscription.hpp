// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/himpl.hpp>
#include <dci/sbs/implMetaInfo.hpp>
#include "api.hpp"
#include "owner.hpp"
#include <cstdint>

namespace dci::sbs
{
    class API_DCI_SBS Subscription
        : public himpl::FaceLayout<Subscription, impl::Subscription>
    {
        Subscription(const Subscription&) = delete;
        void operator=(const Subscription&) = delete;

    public:
        enum Flags
        {
            act4Next    = 0x01,
            act4Last    = 0x02,
            act         = act4Next | act4Last,

            del         = 0x10,
        };

    protected:
        using Activator = void (*)(Subscription*, void *, std::uint_fast8_t);

        Subscription(Activator activator, Owner* owner = nullptr);
        ~Subscription();

        void removeSelf();
    };
}
