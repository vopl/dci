// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../../api.hpp"
#include <dci/stiac/implMetaInfo.hpp>
#include <dci/himpl.hpp>
#include <string>

namespace dci::stiac::link::source
{
    class API_DCI_STIAC Fail
        : public himpl::FaceLayout<Fail, impl::Fail>
    {
        Fail(const Fail&) = delete;
        void operator=(const Fail&) = delete;

    public:
        Fail(const char* cszDetails);
        ~Fail();

        const std::string& details() const;
    };
}
