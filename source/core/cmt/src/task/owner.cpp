// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/cmt/task/owner.hpp>
#include "impl/task/owner.hpp"

namespace dci::cmt::task
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Owner::Owner()
        : himpl::FaceLayout<Owner, impl::task::Owner>()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Owner::~Owner()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Owner::stopRequested() const
    {
        return impl().stopRequested();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Owner::empty() const
    {
        return impl().empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Owner::flush(bool andWait)
    {
        return impl().flush(andWait);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Owner::stop(bool andWait)
    {
        return impl().stop(andWait);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Owner::wait()
    {
        return impl().wait();
    }

}
