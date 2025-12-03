// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/cmt/functions.hpp>
#include "impl/scheduler.hpp"

namespace dci::cmt
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool yield()
    {
        impl::Scheduler& s = impl::Scheduler::instance();

        dbgAssert(s.currentTask());
        if(s.currentTask()->stopRequested())
        {
            throw cmt::task::Stop{};
        }

        return s.yield();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool executeReadyFibers()
    {
        return impl::Scheduler::instance().executeReadyFibers();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void enumerateFibers(FiberEnumerationCallback cb, void* data)
    {
        return impl::Scheduler::instance().enumerateFibers(cb, data);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void spawn(task::Body* task)
    {
        return impl::Scheduler::instance().spawnTask(himpl::face2Impl(task));
    }
}
