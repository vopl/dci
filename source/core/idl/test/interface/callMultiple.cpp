// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include "interface/call.hpp"

#include <vector>
#include <string>

using namespace dci;
using namespace dci::idl;
using namespace dci::idl::gen;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(idl, interface_callMultiple)
{
    {
        h3::Rectangle<> r;
        r.init();
        h3::Rectangle<>::Opposite r2(r);

        EXPECT_EQ(0u, r->width<dci::sbs::wire::Agg::all>().size());

        r2->width() += []
        {
            return dci::cmt::readyFuture(uint32(10));
        };

        EXPECT_EQ(1u, r->width<dci::sbs::wire::Agg::all>().size());

        r2->width() += []
        {
            return dci::cmt::readyFuture(uint32(20));
        };

        EXPECT_EQ(2u, r->width<dci::sbs::wire::Agg::all>().size());
        EXPECT_EQ(2u, r->width<dci::sbs::wire::Agg::all>().size());
    }
}
