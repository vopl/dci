/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/test.hpp>
#include <dci/host.hpp>
#include <dci/crypto/rnd.hpp>
#include <dci/utils/b2h.hpp>
#include <dci/utils/h2b.hpp>
#include <random>

using namespace dci;

//namespace
//{
//    using Id = std::array<uint8, 32>;

//    real64 ld(const Id& base, const Id& target)
//    {
//        Id diff;
//        for(std::size_t i{0}; i<base.size(); ++i)
//        {
//            diff[i] = base[i] ^ target[i];
//        }

//        real64 res = log2(dci::utils::id2Real(diff));
//        dbgAssert(res <= 256);
//        dbgAssert(res >= 0);
//        return res;
//    }

//    Id ld(const Id& base, const real64& target)
//    {
//        Id diff = utils::real2Id<uint8, 32>(exp2(target));

//        Id res;
//        for(std::size_t i{0}; i<res.size(); ++i)
//        {
//            res[i] = base[i] ^ diff[i];
//        }

//        return res;
//    }
//}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_ppn_topology_lis, grid)
{
//    std::random_device rd;
//    std::uniform_real_distribution<> disCenter{1, 255};
//    std::uniform_real_distribution<> disOffset{-1, 1};

//    Id id1, id2;

//    crypto::rnd::generate(id1.data(), id1.size());

//    for(int k{}; k<2000; ++k)
//    {
//        crypto::rnd::generate(id2.data(), id2.size());

//        std::cout<<"dist: "<<ld(id1, id2)<<std::endl;


//        int kk = 20;
//    }
}
