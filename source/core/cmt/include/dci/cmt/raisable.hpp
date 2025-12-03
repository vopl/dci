// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include <dci/himpl.hpp>
#include <dci/cmt/implMetaInfo.hpp>

namespace dci::cmt
{
    class API_DCI_CMT Raisable
        : public himpl::FaceLayout<Raisable, impl::Raisable>
    {
        Raisable(const Raisable&) = delete;
        void operator=(const Raisable&) = delete;

    protected:
        DCI_INTEGRATION_APIDECL_LOCAL Raisable(himpl::FakeConstructionArg fc);
        Raisable() = delete;
        DCI_INTEGRATION_APIDECL_LOCAL ~Raisable();

    public:
        void raise();
    };
}
