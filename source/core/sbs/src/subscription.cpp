// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/sbs/subscription.hpp>
#include <dci/sbs/owner.hpp>
#include "impl/subscription.hpp"
#include "impl/owner.hpp"

namespace dci::sbs
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Subscription::Subscription(Activator activator, Owner* owner)
        : himpl::FaceLayout<Subscription, impl::Subscription>(activator, himpl::face2Impl(owner))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Subscription::~Subscription()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Subscription::removeSelf()
    {
        impl().removeSelf();
    }
}
