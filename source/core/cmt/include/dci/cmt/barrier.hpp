// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "waitable.hpp"
#include <cstdint>

namespace dci::cmt
{
    class API_DCI_CMT Barrier
        : public himpl::FaceLayout<Barrier, impl::Barrier, Waitable>
    {
        Barrier(const Barrier&) = delete;
        void operator=(const Barrier&) = delete;

    public:
        Barrier(std::size_t depth);
        ~Barrier();

        bool canStride() const;
        bool tryStride();
        void stride();

    public:
        void wait();
    };
}
