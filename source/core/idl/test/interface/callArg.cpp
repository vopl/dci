// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include "interface/call.hpp"

#include <vector>
#include <string>

using namespace dci;
using namespace dci::idl;
using namespace dci::idl::gen;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(idl, interface_callArg)
{
    {
        h3::Rectangle<> r;
        r.init();

        h3::Object<>::Opposite o{r};

        int callMarker = 0;

        o->ab()         += [&](bool                       a){callMarker=3;  EXPECT_EQ(a, true);};
        o->ai8()        += [&](int8                       a){callMarker=4;  EXPECT_EQ(a, int8{4});};
        o->ai16()       += [&](int16                      a){callMarker=5;  EXPECT_EQ(a, int16{5});};
        o->ai32()       += [&](int32                      a){callMarker=6;  EXPECT_EQ(a, int32{6});};
        o->ai64()       += [&](int64                      a){callMarker=7;  EXPECT_EQ(a, int64{7});};
        o->aui8()       += [&](uint8                      a){callMarker=8;  EXPECT_EQ(a, uint8{8});};
        o->aui16()      += [&](uint16                     a){callMarker=9;  EXPECT_EQ(a, uint16{9});};
        o->aui32()      += [&](uint32                     a){callMarker=10; EXPECT_EQ(a, uint32{10});};
        o->aui64()      += [&](uint64                     a){callMarker=11; EXPECT_EQ(a, uint64{11});};
        o->ar32()       += [&](real32                     a){callMarker=12; EXPECT_FLOAT_EQ(a, real32{12});};
        o->ar64()       += [&](real64                     a){callMarker=13; EXPECT_FLOAT_EQ(a, real64{13});};
        o->as()         += [&](String                     a){callMarker=14; EXPECT_EQ(a, String{"14"});};
        o->abt()        += [&](Bytes                      a){callMarker=15; EXPECT_EQ(a, Bytes{});};
        o->aiid()       += [&](IId                        a){callMarker=16; EXPECT_EQ(a, IId{});};
        o->aarray()     += [&](Array<String, 2>           a){callMarker=17; EXPECT_EQ(a, (Array<String, 2>{{"17", "18"}}));};
        o->aset()       += [&](Set<String>                a){callMarker=19; EXPECT_EQ(a, (Set<String>{"19", "20"}));};
        o->amap()       += [&](Map<String, Bytes>         a){callMarker=21; EXPECT_EQ(a, (Map<String, Bytes>{{"21", Bytes{}}, {"22", Bytes{}}}));};
        o->alist()      += [&](List<String>               a){callMarker=23; EXPECT_EQ(a, (List<String>{"23", "24"}));};
        o->aptr()       += [&](Ptr<String>                a){callMarker=25; EXPECT_EQ(*a,(*Ptr<String>{new String{"25"}}));};
        o->aopt()       += [&](Opt<String>                a){callMarker=25; EXPECT_EQ(*a,(*Opt<String>{String{"25"}}));};
        o->atuple()     += [&](Tuple<bool_, int8, String> a){callMarker=26; EXPECT_EQ(a, (Tuple<bool_, int8, String>{true, 26, "27"}));};
        o->ainterface() += [&](Interface                  a){callMarker=27; EXPECT_EQ(a, Interface{});};
        o->ax()         += [&](h3::Object<>               a){callMarker=28; EXPECT_EQ(a, h3::Object<>{});};


        for(std::size_t k(0); k<3; ++k)
        {
            {
                r->ab(true);
                EXPECT_EQ(callMarker, 3);
            }

            {
                r->ai8(int8{4});
                EXPECT_EQ(callMarker, 4);
            }

            {
                r->ai16(int16{5});
                EXPECT_EQ(callMarker, 5);
            }

            {
                r->ai32(int32{6});
                EXPECT_EQ(callMarker, 6);
            }

            {
                r->ai64(int64{7});
                EXPECT_EQ(callMarker, 7);
            }

            {
                r->aui8(uint8{8});
                EXPECT_EQ(callMarker, 8);
            }

            {
                r->aui16(uint16{9});
                EXPECT_EQ(callMarker, 9);
            }

            {
                r->aui32(uint32{10});
                EXPECT_EQ(callMarker, 10);
            }

            {
                r->aui64(uint64{11});
                EXPECT_EQ(callMarker, 11);
            }

            {
                r->ar32(real32{12});
                EXPECT_EQ(callMarker, 12);
            }

            {
                r->ar64(real64{13});
                EXPECT_EQ(callMarker, 13);
            }

            {
                r->as(String{"14"});
                EXPECT_EQ(callMarker, 14);
            }

            {
                r->abt(Bytes{});
                EXPECT_EQ(callMarker, 15);
            }

            {
                r->aiid(IId{});
                EXPECT_EQ(callMarker, 16);
            }

            {
                r->aarray(Array<String, 2>{{"17", "18"}});
                EXPECT_EQ(callMarker, 17);
            }

            {
                r->aset(Set<String>{"19", "20"});
                EXPECT_EQ(callMarker, 19);
            }

            {
                r->amap(Map<String, Bytes>{{"21", Bytes{}}, {"22", Bytes{}}});
                EXPECT_EQ(callMarker, 21);
            }

            {
                r->alist(List<String>{"23", "24"});
                EXPECT_EQ(callMarker, 23);
            }

            {
                r->aptr(Ptr<String>{new String{"25"}});
                EXPECT_EQ(callMarker, 25);
            }

            {
                r->aopt(Opt<String>{String{"25"}});
                EXPECT_EQ(callMarker, 25);
            }

            {
                r->atuple(Tuple<bool_, int8, String>{true, 26, "27"});
                EXPECT_EQ(callMarker, 26);
            }

            {
                r->ainterface(Interface{});
                EXPECT_EQ(callMarker, 27);
            }

            {
                r->ax(h3::Object<>{});
                EXPECT_EQ(callMarker, 28);
            }
        }
    }
}
