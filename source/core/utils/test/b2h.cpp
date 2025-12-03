// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/utils/b2h.hpp>
#include <dci/utils/endian.hpp>

using namespace dci::utils;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(utils, b2h)
{
    std::uint32_t bin;

    std::string hex;
    hex.resize(8);

    bin = endian::n2l(0xabcdef00);
    b2h(&bin, 4, hex.data(), HexEndian::little);
    EXPECT_EQ(hex, "00fedcba");

    bin = endian::n2l(0xabcdef00);
    b2h(&bin, 4, hex.data(), HexEndian::middle);
    EXPECT_EQ(hex, "00efcdab");

    bin = endian::n2l(0xabcdef00);
    b2h(&bin, 4, hex.data(), HexEndian::big);
    EXPECT_EQ(hex, "abcdef00");
}
