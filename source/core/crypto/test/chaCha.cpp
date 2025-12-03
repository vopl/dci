// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/crypto.hpp>
#include <dci/utils/b2h.hpp>
#include <dci/utils/h2b.hpp>

using namespace dci::crypto;
using namespace dci::utils;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(crypto, chaCha)
{
    {
        ChaCha h;
        std::vector<uint8_t> key = h2b("000000000000000000000000000000000000000000000000");
        std::string text = "The quick brown fox jumps over the lazy dog";
        h.setKey(key.data(), key.size());
        h.cipher(text.data(), text.data(), text.size());
        EXPECT_EQ(b2h(text.data(), text.size()), "220d58d81d48453fb2d78079c31f3d80bddb1689ac8f08a6bd6108abee50d23b2b42970103d213da31b478");
    }

    {
        ChaCha h;
        std::vector<uint8_t> key = h2b("00000000000000000000000000000000");
        std::string text = "The quick brown fox jumps over the lazy dog";
        h.setKey(key.data(), key.size());
        h.cipher(text.data(), text.data(), text.size());
        EXPECT_EQ(b2h(text.data(), text.size()), "ddf0c627116fd0e9b629b9b79578f58e189399d70d175d93e402d24ec30cf2234a161dd7541151c048909d");
    }

    {
        ChaCha h;
        std::vector<uint8_t> key = h2b("000102030405060708090a0b0c0d0e0f");
        std::vector<uint8_t> iv = h2b("cd00000000000000");
        std::string text = "The quick brown fox jumps over the lazy dog";
        h.setKey(key.data(), key.size());
        h.setIv(iv.data(), iv.size());
        h.cipher(text.data(), text.data(), text.size());
        EXPECT_EQ(b2h(text.data(), text.size()), "dd133a03ebb78479a3607c6857f080782e4cb6e6159bd0ab1c95b0c7f915911031523eb60ad522184caa44");
    }
}
