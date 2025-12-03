// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "base.hpp"

namespace dci::module::ppn::service::aup_legacy_since_2025_04::consumer
{
    class Storage
        : public Base
    {
    public:
        Storage(base::Quota* quota);
        ~Storage() override;

    private:
        bool onComplete(const Oid& oid, base::RecvBuffer& recvBuffer) override;

    private:
        static constexpr int _prioTarget = 10;
        static constexpr int _prioBuffer = 9;
    };
}
