// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/crypto.hpp>
#include <dci/utils/h2b.hpp>

using namespace dci::crypto;
using namespace dci::utils;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(crypto, sha2_512)
{
    std::vector<uint8_t> digest(64);

    {
        Sha2_512 h;
        h.finish(digest.data());
        EXPECT_EQ(digest, h2b("fc381e53e7fe8bdb1f4582056dd608706d024e50b07551cd384f9a123dc69eec740d1dc3d5582f0bff38812d78e7cef2369b13db7414a7185a8323a79f72ade3"));
    }

    {
        Sha2_512 h;
        h.add("The quick brown fox jumps over the lazy dog");
        h.finish(digest.data());
        EXPECT_EQ(digest, h2b("705e749d85f6a6377ff3ab0c34e57d961512f87b0d8c7d883a907d5834b6bb46e2392a259a452f932145d7e1a8b3e56d1efb7d90871232f30a35f8d38b45ef6e"));
    }
}
