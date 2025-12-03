// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include "interface/call.hpp"

#include <vector>
#include <string>

using namespace dci;
using namespace dci::idl;
using namespace dci::idl::gen;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(idl, interface_callRet)
{
    {
        h3::Rectangle<> r;
        r.init();

        h3::Object<>::Opposite o{r};

        int callMarker = 0;

        o->rn()         += [&](){callMarker=1;};
        o->rv()         += [&](){callMarker=2;return dci::cmt::readyFuture(None{});};
        o->rb()         += [&](){callMarker=3;return dci::cmt::readyFuture(true);};
        o->ri8()        += [&](){callMarker=4;return dci::cmt::readyFuture(int8{4});};
        o->ri16()       += [&](){callMarker=5;return dci::cmt::readyFuture(int16{5});};
        o->ri32()       += [&](){callMarker=6;return dci::cmt::readyFuture(int32{6});};
        o->ri64()       += [&](){callMarker=7;return dci::cmt::readyFuture(int64{7});};
        o->rui8()       += [&](){callMarker=8;return dci::cmt::readyFuture(uint8{8});};
        o->rui16()      += [&](){callMarker=9;return dci::cmt::readyFuture(uint16{9});};
        o->rui32()      += [&](){callMarker=10;return dci::cmt::readyFuture(uint32{10});};
        o->rui64()      += [&](){callMarker=11;return dci::cmt::readyFuture(uint64{11});};
        o->rr32()       += [&](){callMarker=12;return dci::cmt::readyFuture(real32{12});};
        o->rr64()       += [&](){callMarker=13;return dci::cmt::readyFuture(real64{13});};
        o->rs()         += [&](){callMarker=14;return dci::cmt::readyFuture(String{"14"});};
        o->rbt()        += [&](){callMarker=15;return dci::cmt::readyFuture(Bytes{});};
        o->riid()       += [&](){callMarker=16;return dci::cmt::readyFuture(IId{});};
        o->rarray()     += [&](){callMarker=17;return dci::cmt::readyFuture(Array<String, 2>{{"17", "18"}});};
        o->rset()       += [&](){callMarker=19;return dci::cmt::readyFuture(Set<String>{"19", "20"});};
        o->rmap()       += [&](){callMarker=21;return dci::cmt::readyFuture(Map<String, Bytes>{{"21", Bytes{}}, {"22", Bytes{}}});};
        o->rlist()      += [&](){callMarker=23;return dci::cmt::readyFuture(List<String>{"23", "24"});};
        o->rptr()       += [&](){callMarker=25;return dci::cmt::readyFuture(Ptr<String>{new String{"25"}});};
        o->ropt()       += [&](){callMarker=25;return dci::cmt::readyFuture(Opt<String>{String{"25"}});};
        o->rtuple()     += [&](){callMarker=26;return dci::cmt::readyFuture(Tuple<bool_, int8, String>{true, 26, "27"});};
        o->rinterface() += [&](){callMarker=27;return dci::cmt::readyFuture(Interface{});};
        o->rx()         += [&](){callMarker=28;return dci::cmt::readyFuture(h3::Object<>{});};


        for(std::size_t k(0); k<3; ++k)
        {
            {
                r->rn();
                EXPECT_EQ(callMarker, 1);
            }

            {
                auto v = r->rv();
                EXPECT_EQ(callMarker, 2);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
            }

            {
                auto v = r->rb();
                EXPECT_EQ(callMarker, 3);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), true);
            }

            {
                auto v = r->ri8();
                EXPECT_EQ(callMarker, 4);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), int8{4});
            }

            {
                auto v = r->ri16();
                EXPECT_EQ(callMarker, 5);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), int16{5});
            }

            {
                auto v = r->ri32();
                EXPECT_EQ(callMarker, 6);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), int32{6});
            }

            {
                auto v = r->ri64();
                EXPECT_EQ(callMarker, 7);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), int64{7});
            }

            {
                auto v = r->rui8();
                EXPECT_EQ(callMarker, 8);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), uint8{8});
            }

            {
                auto v = r->rui16();
                EXPECT_EQ(callMarker, 9);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), uint16{9});
            }

            {
                auto v = r->rui32();
                EXPECT_EQ(callMarker, 10);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), uint32{10});
            }

            {
                auto v = r->rui64();
                EXPECT_EQ(callMarker, 11);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), uint64{11});
            }

            {
                auto v = r->rr32();
                EXPECT_EQ(callMarker, 12);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_FLOAT_EQ(v.value(), real32{12});
            }

            {
                auto v = r->rr64();
                EXPECT_EQ(callMarker, 13);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_FLOAT_EQ(v.value(), real64{13});
            }

            {
                auto v = r->rs();
                EXPECT_EQ(callMarker, 14);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), String{"14"});
            }

            {
                auto v = r->rbt();
                EXPECT_EQ(callMarker, 15);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), Bytes{});
            }

            {
                auto v = r->riid();
                EXPECT_EQ(callMarker, 16);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), IId{});
            }

            {
                auto v = r->rarray();
                EXPECT_EQ(callMarker, 17);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), (Array<String, 2>{{"17", "18"}}));
            }

            {
                auto v = r->rset();
                EXPECT_EQ(callMarker, 19);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), (Set<String>{"19", "20"}));
            }

            {
                auto v = r->rmap();
                EXPECT_EQ(callMarker, 21);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), (Map<String, Bytes>{{"21", Bytes{}}, {"22", Bytes{}}}));
            }

            {
                auto v = r->rlist();
                EXPECT_EQ(callMarker, 23);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), (List<String>{"23", "24"}));
            }

            {
                auto v = r->rptr();
                EXPECT_EQ(callMarker, 25);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(*v.value(), (*Ptr<String>{new String{"25"}}));
            }

            {
                auto v = r->ropt();
                EXPECT_EQ(callMarker, 25);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), (Opt<String>{String{"25"}}));
            }

            {
                auto v = r->rtuple();
                EXPECT_EQ(callMarker, 26);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), (Tuple<bool_, int8, String>{true, 26, "27"}));
            }

            {
                auto v = r->rinterface();
                EXPECT_EQ(callMarker, 27);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), (Interface{}));
            }

            {
                auto v = r->rx();
                EXPECT_EQ(callMarker, 28);
                EXPECT_TRUE(v.resolved());
                EXPECT_TRUE(v.waitValue());
                EXPECT_EQ(v.value(), (h3::Object<>{}));
            }
        }
    }
}
