/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "pch.hpp"

namespace dci::module::www
{
    class ChannelSoftClosing
    {
    public:
        void push(api::stream::Channel<>&& target);

    public:
        static void moduleStarted();
        static ChannelSoftClosing& instance();
        static void moduleStopRequested();
        static void moduleStopped();

    protected:
        ChannelSoftClosing();
        ~ChannelSoftClosing();

    protected:
        struct Channel
        {
            Channel(api::stream::Channel<>&& target)
                : _target{std::move(target)}
            {
            }
            ~Channel()
            {
                _sol.flush();
            }

            api::stream::Channel<>  _target;
            mutable poll::Timer     _timer{std::chrono::milliseconds{ 15000 }};
            mutable sbs::Owner      _sol;
        };

        struct ChannelCmp
        {
            using is_transparent = void;
            bool operator()(const Channel& a, const Channel& b) const
            {
                return a._target < b._target;
            }

            bool operator()(const Channel& a, const api::stream::Channel<>& b) const
            {
                return a._target < b;
            }

            bool operator()(const api::stream::Channel<>& a, const Channel& b) const
            {
                return a < b._target;
            }
        };

        std::set<Channel, ChannelCmp> _channels;
    };
}
