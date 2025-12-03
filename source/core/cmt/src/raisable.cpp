// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/cmt/raisable.hpp>
#include "impl/raisable.hpp"

namespace dci::cmt
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Raisable::Raisable(himpl::FakeConstructionArg fc)
        : FaceLayout(fc)
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Raisable::~Raisable()
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Raisable::raise()
    {
        impl().raise();
    }
}
