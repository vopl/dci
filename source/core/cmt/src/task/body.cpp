// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/cmt/task/body.hpp>
#include <dci/cmt/task/owner.hpp>
#include "impl/task/body.hpp"
#include "../impl/scheduler.hpp"

namespace dci::cmt::task
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Body::Body(Owner* owner, CallAndDestroyExecutor callAndDestroyExecutor)
        : himpl::FaceLayout<Body, impl::task::Body>(himpl::face2Impl(owner), callAndDestroyExecutor)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Body::~Body()
    {
    }
}
