// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/crypto.hpp>
#include <dci/utils/h2b.hpp>

using namespace dci::crypto;
using namespace dci::utils;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(crypto, hmac)
{
    std::vector<uint8_t> digest(32);

    {
        Hmac h(Sha2_256::alloc());
        h.setKey("", 0);
        h.finish(digest.data());
        EXPECT_EQ(digest, h2b("6b3176a980419dce77f2597d873cf55cff61794c391765356c7c214124295cda"));
    }

    {
        Hmac h(Sha2_256::alloc());
        h.setKey("key", 3);
        h.add("The quick brown fox jumps over the lazy dog");
        h.finish(digest.data());
        EXPECT_EQ(digest, h2b("7fcb384f033548421b23896eaaf61b34fed4951a946471957974d9cbd2a1c38d"));
    }
}
