// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "root.hpp"
#include "fiber.hpp"

namespace dci::cmt::impl::ctx
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Root::Root()
    {
        constructRoot();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Root::~Root()
    {
        destructRoot();
    }
}
