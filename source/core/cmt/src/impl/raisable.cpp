// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "impl/raisable.hpp"

namespace dci::cmt::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Raisable::Raisable(void (* raise)(Raisable *))
        : _raise(raise)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Raisable::~Raisable()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Raisable::tryDestruction(Raisable*)
    {
        //empty is ok
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Raisable::raise()
    {
        _raise(this);
    }
}
