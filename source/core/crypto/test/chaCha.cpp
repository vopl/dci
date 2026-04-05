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
