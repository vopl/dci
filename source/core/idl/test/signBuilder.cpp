// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/utils/b2h.hpp>

#include "../im/signBuilder.hpp"
#include "../im/sign.hpp"

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(idl, signBuilder)
{
    using namespace dci::idl::im;

    {
        SignBuilder sb;
        EXPECT_EQ("e075150c625e342b8ebae20b0699ad1a1d5efd7477f87778afba54dc1ff23e8a", dci::utils::b2h(sb.finish()));
    }

    {
        SignBuilder sb;
        sb.add("The quick brown fox jumps over the lazy dog");
        EXPECT_EQ("86abaaace75421a366ab7d9f683c630fd04ebed4218549cfe45ef4ccc5800bce", dci::utils::b2h(sb.finish()));
    }

    {
        SignBuilder sb;
        sb.add("8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c928d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c9");
        EXPECT_EQ("ffec5e5e971c3cf0eba247431dd67ff37206303ac1a2e42a7e0b393607f5d12c", dci::utils::b2h(sb.finish()));
    }

    {
        SignBuilder sb;

        sb.add(SignBuilder{}.finish());
        sb.add(std::string("std::string"));
        sb.add("csz");

        sb.add(true);

        sb.add(std::uint8_t(1));
        sb.add(std::uint16_t(2));
        sb.add(std::uint32_t(3));
        sb.add(std::uint64_t(4));

        sb.add(std::int8_t(5));
        sb.add(std::int16_t(6));
        sb.add(std::int32_t(7));
        sb.add(std::int64_t(8));

        EXPECT_EQ("222915eedb8ee382f0062762f266575188aac6f69d52fc367ae22881b397c88f", dci::utils::b2h(sb.finish()));
    }
}
