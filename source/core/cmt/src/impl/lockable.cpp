// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "lockable.hpp"
#include "scheduler.hpp"
#include <dci/cmt/task/stop.hpp>

namespace dci::cmt::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Lockable::Lockable(
            bool (* canLock)(const Waitable* lockableBase),
            bool (* tryLock)(Waitable* lockableBase),
            void (* unlock)(Lockable*))
        : Waitable(canLock, tryLock)
        , _unlock(unlock)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Lockable::~Lockable()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lockable::tryDestruction(Lockable *)
    {
        //empty is ok
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Lockable::canLock() const
    {
        return Waitable::canAcquire();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Lockable::tryLock()
    {
        return Waitable::tryAcquire();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lockable::lock()
    {
        return Waitable::wait();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lockable::unlock()
    {
        _unlock(this);
    }

}
