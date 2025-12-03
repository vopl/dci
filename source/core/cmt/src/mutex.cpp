// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/cmt/mutex.hpp>
#include "impl/mutex.hpp"

namespace dci::cmt
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mutex::Mutex(RecursionMode recursionMode)
        : FaceLayout(recursionMode)
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mutex::~Mutex()
    {

    }
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Mutex::canLock() const
    {
        return impl().canLock();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Mutex::tryLock()
    {
        return impl().tryLock();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Mutex::lock()
    {
        return impl().lock();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Mutex::unlock()
    {
        return impl().unlock();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Mutex::wait()
    {
        return impl().wait();
    }
}
