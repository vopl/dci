// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include <dci/cmt/api.hpp>
#include <dci/himpl.hpp>
#include <dci/cmt/implMetaInfo.hpp>

namespace dci::cmt::task
{
    class Owner;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class API_DCI_CMT Body
        : public himpl::FaceLayout<Body, impl::task::Body>
    {
        Body() = delete;
        Body(const Body&) = delete;
        void operator=(const Body&) = delete;

    protected:
        using CallAndDestroyExecutor = void (*)(Body* task, bool call, bool destroy) noexcept(true);

    protected:
        Body(Owner* owner, CallAndDestroyExecutor callAndDestroyExecutor);
        ~Body();
    };
}
