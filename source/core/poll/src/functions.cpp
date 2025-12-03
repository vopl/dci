// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/poll/functions.hpp>
#include <dci/poll/error.hpp>
#include "impl/service.hpp"

namespace dci::poll
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code initialize()
    {
        return impl::service.initialize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code run(bool emitStartedStopped)
    {
        return impl::service.run(emitStartedStopped);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<> started()
    {
        return impl::service.started();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<bool> doSomeWork()
    {
        return impl::service.doSomeWork();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code stop()
    {
        return impl::service.stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<> stopped()
    {
        return impl::service.stopped();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code deinitialize()
    {
        return impl::service.deinitialize();
    }
}
