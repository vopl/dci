// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/cmt/semaphore.hpp>
#include "impl/semaphore.hpp"

namespace dci::cmt
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Semaphore::Semaphore(std::size_t depth)
        : FaceLayout(depth)
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Semaphore::~Semaphore()
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Semaphore::canLock() const
    {
        return impl().canLock();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Semaphore::tryLock()
    {
        return impl().tryLock();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Semaphore::lock()
    {
        return impl().lock();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Semaphore::unlock()
    {
        return impl().unlock();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Semaphore::wait()
    {
        return impl().wait();
    }
}
