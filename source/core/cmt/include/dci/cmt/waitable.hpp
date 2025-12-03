// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include <dci/himpl.hpp>
#include <dci/cmt/implMetaInfo.hpp>

namespace dci::cmt
{
    class API_DCI_CMT Waitable
        : public himpl::FaceLayout<Waitable, impl::Waitable>
    {
        Waitable(const Waitable&) = delete;
        void operator=(const Waitable&) = delete;

    protected:
        DCI_INTEGRATION_APIDECL_LOCAL Waitable(himpl::FakeConstructionArg fc);
        Waitable() = delete;
        DCI_INTEGRATION_APIDECL_LOCAL ~Waitable();

    public:
        void wait();
    };
}
