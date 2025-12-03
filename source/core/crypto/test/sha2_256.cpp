// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/crypto.hpp>
#include <dci/utils/h2b.hpp>

using namespace dci::crypto;
using namespace dci::utils;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(crypto, sha2_256)
{
    std::vector<uint8_t> digest(32);

    {
        Sha2_256 h;
        h.finish(digest.data());
        EXPECT_EQ(digest, h2b("3e0b4c2489cfc141a9bf4f8c99f69b4272ea144e46b939c44a5999b187258b55"));
    }

    {
        Sha2_256 h;
        h.add("The quick brown fox jumps over the lazy dog");
        h.finish(digest.data());
        EXPECT_EQ(digest, h2b("7d8abf3b707d084996aca9cb0b80e2f4d865154ed6c3bd67d2200dfb739c5e29"));
    }
}
