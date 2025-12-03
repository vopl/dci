// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/cmt/notifier.hpp>
#include "impl/notifier.hpp"

namespace dci::cmt
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Notifier::Notifier(WakeMode wakeMode)
        : FaceLayout(wakeMode)
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Notifier::~Notifier()
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Notifier::isRaised() const
    {
        return impl().isRaised();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Notifier::reset()
    {
        return impl().reset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Notifier::wait()
    {
        return impl().wait();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Notifier::raise()
    {
        return impl().raise();
    }
}
