// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/cmt/pulser.hpp>
#include "impl/pulser.hpp"

namespace dci::cmt
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Pulser::Pulser(WakeMode wakeMode)
        : FaceLayout(wakeMode)
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Pulser::~Pulser()
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Pulser::wait()
    {
        return impl().wait();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Pulser::raise()
    {
        return impl().raise();
    }
}

