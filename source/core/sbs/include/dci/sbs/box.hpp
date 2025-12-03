// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/himpl.hpp>
#include <dci/sbs/implMetaInfo.hpp>
#include "api.hpp"
#include "subscription.hpp"

namespace dci::sbs
{
    class API_DCI_SBS Box
        : public himpl::FaceLayout<Box, impl::Box>
    {
        Box(const Box&) = delete;
        void operator=(const Box&) = delete;

    public:
        enum Flags
        {
            //actFirst    = 0x21,
            //actAll      = 0x22,
            del         = 0x30,
        };

    public:
        Box();
        ~Box();

        bool empty() const;
        void push(Subscription* subscription);
        void removeAndDelete(Subscription* subscription);
        void removeAndDeleteAll();
        void activate(void* context = nullptr, std::uint_fast8_t flags = 0);
    };
}
