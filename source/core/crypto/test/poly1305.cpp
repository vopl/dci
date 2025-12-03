// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/crypto.hpp>
#include <dci/utils/h2b.hpp>

using namespace dci::crypto;
using namespace dci::utils;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(crypto, poly1305)
{
    std::vector<uint8_t> digest(16);

    {
        Poly1305 h;
        h.setKey("", 0);
        h.add(h2b("00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
        h.finish(digest.data());
        EXPECT_EQ(digest, h2b("00000000000000000000000000000000"));
    }

    {
        Poly1305 h;
        h.setKey("key", 3);
        h.add("The quick brown fox jumps over the lazy dog");
        h.finish(digest.data());
        EXPECT_EQ(digest, h2b("f690f6fdc3f27b60b12fbf2423fc21ad"));
    }
}
