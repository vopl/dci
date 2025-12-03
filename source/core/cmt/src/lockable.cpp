// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/cmt/lockable.hpp>
#include "impl/lockable.hpp"

namespace dci::cmt
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Lockable::Lockable(himpl::FakeConstructionArg fc)
        : FaceLayout(fc)
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Lockable::~Lockable()
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Lockable::canLock() const
    {
        return impl().canLock();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Lockable::tryLock()
    {
        return impl().tryLock();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lockable::lock()
    {
        return impl().lock();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lockable::unlock()
    {
        return impl().unlock();
    }
}
