// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include "interface/call.hpp"

#include <vector>
#include <string>

using namespace dci::idl;
using namespace dci::idl::gen;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(idl, interface_callUnconnected)
{
    {
        h3::Rectangle<> r;
        r.init();

        EXPECT_NO_THROW(r->width());
        auto res = r->width();

        EXPECT_TRUE(res.resolvedException());

        EXPECT_THROW(res.value(), interface::exception::MethodNotConnected);
    }
}
