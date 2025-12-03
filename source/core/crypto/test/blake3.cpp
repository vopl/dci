// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/crypto.hpp>
#include <dci/utils/h2b.hpp>
#include <dci/utils/b2h.hpp>

#include <iostream>

using namespace dci::crypto;
using namespace dci::utils;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(crypto, blake3)
{
    std::vector<uint8_t> digest(32);

    {
        Blake3 h;
        h.finish(digest.data());
        EXPECT_EQ(digest, h2b("fa31949b5f9f1a6a0a04d4ae63cd9c94b9bc529cda1c217bcca939ac4ef12326"));
    }

    {
        Blake3 h;
        h.add("The quick brown fox jumps over the lazy dog");
        h.finish(digest.data());
        EXPECT_EQ(digest, h2b("f2514181a1dacc9d31ba9dc4af9572105a86a62bf3d81ffd1f7b7401efcbd6a4"));
    }
}
