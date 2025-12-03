// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/crypto.hpp>
#include <dci/utils/h2b.hpp>

using namespace dci::crypto;
using namespace dci::utils;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(crypto, ed25519)
{
    char secret[32] = "0123456789012345678901234567890";
    char pub[32];
    char msg[] = "The quick brown fox jumps over the lazy dog";
    char signature[64];

    ed25519::mkPublic(secret, pub);

    ed25519::sign(msg, sizeof(msg), pub, secret, signature);

    msg[0] = ~msg[0];
    EXPECT_FALSE(ed25519::verify(msg, sizeof(msg), pub, signature));
    msg[0] = ~msg[0];


    pub[0] = ~pub[0];
    EXPECT_FALSE(ed25519::verify(msg, sizeof(msg), pub, signature));
    pub[0] = ~pub[0];

    signature[0] = ~signature[0];
    EXPECT_FALSE(ed25519::verify(msg, sizeof(msg), pub, signature));
    signature[0] = ~signature[0];

    EXPECT_TRUE(ed25519::verify(msg, sizeof(msg), pub, signature));
}
