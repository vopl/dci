// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/utils/h2b.hpp>
#include <dci/utils/endian.hpp>

using namespace dci::utils;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(utils, h2b)
{
    std::uint32_t bin;

    EXPECT_TRUE(h2b("abcdef00", 8, &bin, HexEndian::little));
    EXPECT_EQ(endian::l2n(bin), std::uint32_t{0x00fedcba});

    EXPECT_TRUE(h2b("abcdef00", 8, &bin, HexEndian::middle));
    EXPECT_EQ(endian::l2n(bin), std::uint32_t{0x00efcdab});

    EXPECT_TRUE(h2b("abcdef00", 8, &bin, HexEndian::big));
    EXPECT_EQ(endian::l2n(bin), std::uint32_t{0xabcdef00});
}
