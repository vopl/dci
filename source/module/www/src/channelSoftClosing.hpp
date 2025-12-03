// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
