// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/crypto.hpp>
#include <dci/utils/h2b.hpp>

using namespace dci::crypto;
using namespace dci::utils;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(crypto, blake2s)
{
    std::vector<uint8_t> digest(32);

    {
        Blake2s h;
        h.finish(digest.data());
        EXPECT_EQ(digest, h2b("9612a703970908491e11120d2453a4c7f1556b84c21a5ae1b152d0dfe10dee9f"));
    }

    {
        Blake2s h;
        h.add("The quick brown fox jumps over the lazy dog");
        h.finish(digest.data());
        EXPECT_EQ(digest, h2b("06b6eece47c3bcfe6fbcdc5f5d03a28a552c652cb9888cde33e11a6afbc38821"));
    }
}
