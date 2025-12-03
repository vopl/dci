// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/test.hpp>
#include <dci/poll.hpp>
#include <dci/cmt.hpp>
#include <dci/sbs.hpp>

namespace utils
{
    inline void complexRun()
    {
        EXPECT_FALSE(dci::poll::initialize());

        {
            dci::sbs::Owner doSomeWorkOwner;
            dci::poll::doSomeWork() += doSomeWorkOwner * [&]
            {
                return dci::cmt::executeReadyFibers();
            };
            EXPECT_FALSE(dci::poll::run());
        }

        EXPECT_FALSE(dci::poll::deinitialize());
    }
}
