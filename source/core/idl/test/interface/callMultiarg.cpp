// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include "interface/call.hpp"

#include <vector>
#include <string>

using namespace dci;
using namespace dci::idl;
using namespace dci::idl::gen;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(idl, interface_callMultiarg)
{
    {
        h3::Rectangle<> r;
        r.init();

        h3::Object<>::Opposite o{r};

        int callMarker = 0;

        o->multiarg() += [&](
            bool                       a3,
            int8                       a4,
            int16                      a5,
            int32                      a6,
            int64                      a7,
            uint8                      a8,
            uint16                     a9,
            uint32                     a10,
            uint64                     a11,
            real32                     a12,
            real64                     a13,
            String                     a14,
            Bytes                      a15,
            IId                        a16,
            Array<String, 2>           a17,
            Set<String>                a19,
            Map<String, Bytes>         a21,
            List<String>               a23,
            Ptr<String>                a25,
            Opt<String>                a26,
            Tuple<bool_, int8, String> a27,
            Interface                  a28,
            h3::Object<>               a29)
        {
            callMarker=42;

            EXPECT_EQ(a3, true);
            EXPECT_EQ(a4, int8{4});
            EXPECT_EQ(a5, int16{5});
            EXPECT_EQ(a6, int32{6});
            EXPECT_EQ(a7, int64{7});
            EXPECT_EQ(a8, uint8{8});
            EXPECT_EQ(a9, uint16{9});
            EXPECT_EQ(a10, uint32{10});
            EXPECT_EQ(a11, uint64{11});
            EXPECT_FLOAT_EQ(a12, real32{12});
            EXPECT_FLOAT_EQ(a13, real64{13});
            EXPECT_EQ(a14, String{"14"});
            EXPECT_EQ(a15, Bytes{});
            EXPECT_EQ(a16, IId{});
            EXPECT_EQ(a17, (Array<String, 2>{{"17", "18"}}));
            EXPECT_EQ(a19, (Set<String>{"19", "20"}));
            EXPECT_EQ(a21, (Map<String, Bytes>{{"21", Bytes{}}, {"22", Bytes{}}}));
            EXPECT_EQ(a23, (List<String>{"23", "24"}));
            EXPECT_EQ(*a25,(*Ptr<String>{new String{"25"}}));
            EXPECT_EQ(a26, (Opt<String>{String{"25"}}));
            EXPECT_EQ(a27, (Tuple<bool_, int8, String>{true, 26, "27"}));
            EXPECT_EQ(a28, Interface{});
            EXPECT_EQ(a29, h3::Object<>{});
        };


        for(std::size_t k(0); k<3; ++k)
        {
            r->multiarg(
                true,
                int8{4},
                int16{5},
                int32{6},
                int64{7},
                uint8{8},
                uint16{9},
                uint32{10},
                uint64{11},
                real32{12},
                real64{13},
                String{"14"},
                Bytes{},
                IId{},
                Array<String, 2>{{"17", "18"}},
                Set<String>{"19", "20"},
                Map<String, Bytes>{{"21", Bytes{}}, {"22", Bytes{}}},
                List<String>{"23", "24"},
                Ptr<String>{new String{"25"}},
                Opt<String>{String{"25"}},
                Tuple<bool_, int8, String>{true, 26, "27"},
                Interface{},
                h3::Object<>{}
            );

            EXPECT_EQ(callMarker, 42);
        }
    }
}
