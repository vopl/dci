/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/test.hpp>
#include <dci/crypto.hpp>
#include <dci/utils/b2h.hpp>
#include <dci/utils/h2b.hpp>

using namespace dci::crypto;
using namespace dci::utils;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(crypto, chaCha20Poly1305)
{
    {
        ChaCha20Poly1305 a;
        std::vector<uint8_t> key = h2b("000000000000000000000000000000000000000000000000");
        std::string text = "The quick brown fox jumps over the lazy dog";
        std::vector<uint8_t> mac(16);
        //a.setKey(key.data(), key.size());

        a.start(nullptr, 0);
        a.encipher(text.data(), text.data(), text.size());
        a.encipherFinish(mac.data());

        EXPECT_EQ(b2h(text.data(), text.size()), "bcf628e9424215913fa95fe0c1a566d2da06150822698091166ec38475c9a599142d10a1dfc973361be145");
        EXPECT_EQ(b2h(mac.data(), mac.size()), "297e1a143b9ffd6ddcf57fa7f34fc24b");
    }

    {
        ChaCha20Poly1305 a;
        std::vector<uint8_t> key = h2b("000102030405060708090a0b0c0d0e0f");
        std::vector<uint8_t> ad = h2b("0123456789abcdef");
        std::vector<uint8_t> nonce = h2b("fedcba9876543210");
        std::string text = "The quick brown fox jumps over the lazy dog";
        std::vector<uint8_t> mac(16);

        a.setKey(key.data(), key.size());
        a.setAd(ad.data(), ad.size());
        a.start(nonce.data(), nonce.size());
        a.encipher(text.data(), text.data(), text.size());
        a.encipherFinish(mac.data());

        EXPECT_EQ(b2h(text.data(), text.size()), "4d47deb9d36464ecb7b08bc0365ce5d8c5e67a8af058b811a9d38f6ccfff094d1e1e9ba53069f7e0ea3a50");
        EXPECT_EQ(b2h(mac.data(), mac.size()), "c62d5c2e8af841664d18f7b82c6696e8");
    }

    {
        ChaCha20Poly1305 a;
        std::vector<uint8_t> key = h2b("000102030405060708090a0b0c0d0e0f");
        std::vector<uint8_t> ad = h2b("0123456789abcdef");
        std::vector<uint8_t> nonce = h2b("fedcba9876543210");
        std::string text;

        std::vector<uint8_t> ctext = h2b("4d47deb9d36464ecb7b08bc0365ce5d8c5e67a8af058b811a9d38f6ccfff094d1e1e9ba53069f7e0ea3a50");
        std::vector<uint8_t> mac = h2b("c62d5c2e8af841664d18f7b82c6696e8");

        a.setKey(key.data(), key.size());
        a.setAd(ad.data(), ad.size());
        a.start(nonce.data(), nonce.size());

        text.resize(ctext.size());
        a.decipher(ctext.data(), text.data(), ctext.size());
        bool res = a.decipherFinish(mac.data());

        EXPECT_TRUE(res);
        EXPECT_EQ(text, "The quick brown fox jumps over the lazy dog");
    }

    {
        ChaCha20Poly1305 a;
        std::vector<uint8_t> key = h2b("000102030405060708090a0b0c0d0e0f");
        std::vector<uint8_t> ad = h2b("0123456789abcdef");
        std::vector<uint8_t> nonce = h2b("fedcba9876543210");
        std::string text;

        std::vector<uint8_t> ctext = h2b("4d47deb9d36464ecb7b08bc0365ce5d8c5e67a8af058b811a9d38f6ccfff094d1e1e9ba53069f7e0ea3a50");
        ctext[3] ^= 1;
        std::vector<uint8_t> mac = h2b("c62d5c2e8af841664d18f7b82c6696e8");

        a.setKey(key.data(), key.size());
        a.setAd(ad.data(), ad.size());
        a.start(nonce.data(), nonce.size());

        text.resize(ctext.size());
        a.decipher(ctext.data(), text.data(), ctext.size());
        bool res = a.decipherFinish(mac.data());

        EXPECT_FALSE(res);
    }

}
