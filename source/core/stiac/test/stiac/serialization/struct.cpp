// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include "utils/hereThere.hpp"
using namespace utils;

#include <dci/idl/introspection.hpp>

using namespace dci;
using namespace dci::idl;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
struct StructVictimBase
{
    real32 bfld1;
};

namespace dci::idl::introspection
{
    template <> inline constexpr Kind kind<StructVictimBase> = Kind::struct_;
    template <> inline constexpr uint32 basesCount<StructVictimBase> = 0;
    template <> inline constexpr uint32 fieldsCount<StructVictimBase> = 1;
    template <> struct FieldType<StructVictimBase, 0> {using result = real32; };
    template <> inline constexpr auto fieldValue<StructVictimBase, 0> = memberValue<StructVictimBase, &StructVictimBase::bfld1>;
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
struct StructVictim : public StructVictimBase
{
    int fld1;
    std::string fld2;
};

namespace dci::idl::introspection
{
    template <> inline constexpr Kind kind<StructVictim> = Kind::struct_;
    template <> inline constexpr uint32 basesCount<StructVictim> = 1;
    template <> struct BaseType<StructVictim, 0> {using result = StructVictimBase; };
    template <> inline constexpr uint32 fieldsCount<StructVictim> = 2;
    template <> struct FieldType<StructVictim, 0> {using result = int; };
    template <> struct FieldType<StructVictim, 1> {using result = std::string; };
    template <> inline constexpr auto fieldValue<StructVictim, 0> = memberValue<StructVictim, &StructVictim::fld1>;
    template <> inline constexpr auto fieldValue<StructVictim, 1> = memberValue<StructVictim, &StructVictim::fld2>;
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(stiac, struct)
{
    hereThere2( StructVictimBase{2.34567f}   ,[&](const auto&, const auto& b)
    {
        EXPECT_FLOAT_EQ(b.bfld1, 2.34567);
    });

    hereThere2( StructVictim{{1.234f}, 42, "42"}   ,[&](const auto& , const auto& b)
    {
        EXPECT_FLOAT_EQ(b.bfld1, 1.234);
        EXPECT_EQ(b.fld1, 42);
        EXPECT_EQ(b.fld2, "42");
    });
}
