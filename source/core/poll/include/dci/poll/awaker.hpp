/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/poll/api.hpp>
#include <dci/himpl.hpp>
#include <dci/poll/implMetaInfo.hpp>
#include <dci/cmt/task/owner.hpp>
#include <dci/cmt/raisable.hpp>
#include <dci/sbs/signal.hpp>

#include <concepts>

namespace dci::poll
{
    class API_DCI_POLL Awaker
        : public himpl::FaceLayout<Awaker, impl::Awaker>
    {
        Awaker(const Awaker&) = delete;
        void operator=(const Awaker&) = delete;

    public:
        Awaker(bool keepLoop = true);
        Awaker(cmt::task::Owner* wokenOwner, bool keepLoop = true);
        Awaker(cmt::Raisable* raisable, bool keepLoop = true);
        explicit Awaker(auto&& onWoken, bool keepLoop = true) requires(std::invocable<decltype(onWoken)&&>);
        explicit Awaker(auto&& onWoken, cmt::task::Owner* wokenOwner, bool keepLoop = true) requires(std::invocable<decltype(onWoken)&&>);

        ~Awaker();

        bool keepLoop() const;

        sbs::Signal<> woken();

        void setWokenOwner(cmt::task::Owner* wokenOwner);
        void resetWokenOwner();

        void setRaisable(cmt::Raisable* raisable);
        void resetRaisable();

        void wakeup();
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Awaker::Awaker(auto&& onWoken, bool keepLoop) requires(std::invocable<decltype(onWoken)&&>)
        : Awaker{keepLoop}
    {
        this->woken() += std::forward<decltype(onWoken)>(onWoken);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Awaker::Awaker(auto&& onWoken, cmt::task::Owner* wokenOwner, bool keepLoop) requires(std::invocable<decltype(onWoken)&&>)
        : Awaker{wokenOwner, keepLoop}
    {
        this->woken() += std::forward<decltype(onWoken)>(onWoken);
    }
}
