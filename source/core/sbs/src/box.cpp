// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/sbs/box.hpp>
#include "impl/box.hpp"

namespace dci::sbs
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Box::Box()
        : himpl::FaceLayout<Box, impl::Box>()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Box::~Box()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Box::empty() const
    {
        return impl().empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Box::push(Subscription* subscription)
    {
        return impl().push(himpl::face2Impl(subscription));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Box::removeAndDelete(Subscription* subscription)
    {
        return impl().removeAndDelete(himpl::face2Impl(subscription));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Box::removeAndDeleteAll()
    {
        return impl().removeAndDeleteAll();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Box::activate(void* context, std::uint_fast8_t flags)
    {
        return impl().activate(context, flags);
    }
}
