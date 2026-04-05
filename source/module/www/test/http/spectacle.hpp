/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/test.hpp>
#include <dci/host.hpp>
#include <dci/poll.hpp>
#include "www.hpp"
#include "utils.hpp"

using namespace dci;
using namespace dci::host;
using namespace dci::idl;

namespace http
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool forServer>
    struct Spectacle
    {
        using Target = std::conditional_t<
            forServer,
            www::http::server::Channel<>,
            www::http::client::Channel<>>;

        using Input = std::conditional_t<
            forServer,
            www::http::server::Request<>,
            www::http::client::Response<>>;

        using Output = std::conditional_t<
            forServer,
            www::http::server::Response<>,
            www::http::client::Request<>>;

        struct Failed           : Tuple<std::string>                               { using Tuple::Tuple; };
        struct Closed           : Tuple<>                                          { using Tuple::Tuple; };
        struct Io               : Tuple<>                                          { using Tuple::Tuple; };

        struct InputFailed      : Tuple<std::string>                               { using Tuple::Tuple; };
        struct InputClosed      : Tuple<>                                          { using Tuple::Tuple; };
        struct InputFirstLine4S : Tuple<www::http::firstLine::Method, primitives::String, www::http::firstLine::Version> { using Tuple::Tuple; };
        struct InputFirstLine4C : Tuple<www::http::firstLine::Version, primitives::uint16, primitives::String> { using Tuple::Tuple; };
        struct InputHeaders     : Tuple<primitives::List<www::http::Header>, bool> { using Tuple::Tuple; };
        struct InputData        : Tuple<Bytes, bool>                               { using Tuple::Tuple; };
        struct InputDone        : Tuple<>                                          { using Tuple::Tuple; };

        struct OutputFailed     : Tuple<std::string>                               { using Tuple::Tuple; };
        struct OutputClosed     : Tuple<>                                          { using Tuple::Tuple; };

        struct PeerFailed       : Tuple<std::string>                               { using Tuple::Tuple; };
        struct PeerClosed       : Tuple<>                                          { using Tuple::Tuple; };
        struct PeerData         : Tuple<Bytes>                                     { using Tuple::Tuple; };

        using Action = primitives::Variant
        <
            Failed,
            Closed,
            Io,

            InputFailed,
            InputClosed,
            InputFirstLine4S,
            InputFirstLine4C,
            InputHeaders,
            InputData,
            InputDone,

            OutputFailed,
            OutputClosed,

            PeerFailed,
            PeerClosed,
            PeerData
        >;

        std::deque<Action>  _actions;
        Bytes               _peerDataMerged;
        Target              _target;

        struct IO
        {
            Input   _input;
            Output  _output;
        };
        std::deque<IO>                  _ios;
        www::stream::Channel<>          _peer;
        sbs::Owner                      _sol;

        Spectacle(www::http::server::Channel<>&& target, www::stream::Channel<>&& peer)
            : _target(std::move(target))
            , _peer(std::move(peer))
        {
            interconnect();
        }

        Spectacle()
        {
            www::stream::Channel<>::Opposite targetStream;
            std::tie(_peer, targetStream) = http::utils::interconnectWwwStream();

            www::Factory<> wwwFactory = *testManager()->createService<www::Factory<>>();
            if constexpr (forServer)
                _target = *wwwFactory->stream2HttpServer(targetStream);
            else
                _target = *wwwFactory->stream2HttpClient(targetStream);

            interconnect();
        }

        ~Spectacle()
        {
            _sol.flush();
        }

        void interconnect()
        {
            _target->failed() += _sol * [&](dci::primitives::ExceptionPtr&& e)
            {
                _actions.emplace_back(Failed{dci::exception::toString(e)});
            };

            _target->closed() += _sol * [&]()
            {
                _actions.emplace_back(Closed{});
            };

            if constexpr(forServer)
            {
                _target->io() += _sol * [this](Input&& input, Output&& output)
                {
                    connectIo(std::move(input), std::move(output));
                };
            }

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            _peer->failed() += _sol * [&](dci::primitives::ExceptionPtr&& e)
            {
                _actions.emplace_back(PeerFailed{dci::exception::toString(e)});
            };

            _peer->closed() += _sol * [&]()
            {
                _actions.emplace_back(PeerClosed{});
            };

            _peer->received() += _sol * [&](Bytes&& data)
            {
                _peerDataMerged.end().write(data);
                _actions.emplace_back(PeerData{ std::move(data) });
            };

            _peer->startReceive();
        }

        void startIo() requires(!forServer)
        {
            Input input;
            Output output;
            connectIo(input.init2(), output.init2());
            _target->io(std::move(output), std::move(input));
        }

        void connectIo(Input&& input, Output&& output)
        {
            _actions.emplace_back(Io{});

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            input->setupDataProcessing(
                        www::http::message::tunable::Compression::byHeaders,
                        www::http::message::tunable::Compression::byHeaders,
                        www::http::message::tunable::Encoding::byHeaders);

            input->failed() += _sol * [&](dci::primitives::ExceptionPtr&& e)
            {
                _actions.emplace_back(InputFailed{dci::exception::toString(e)});
            };

            input->closed() += _sol * [&]()
            {
                _actions.emplace_back(InputClosed{});
            };

            if constexpr(forServer)
            {
                input->firstLine() += _sol * [&](www::http::firstLine::Method method, primitives::String&& uri, www::http::firstLine::Version version)
                {
                    _actions.emplace_back(InputFirstLine4S{ method, std::move(uri), version});
                };
            }
            else
            {
                input->firstLine() += _sol * [&](www::http::firstLine::Version version, primitives::uint16 statusCode, primitives::String&& statusText)
                {
                    _actions.emplace_back(InputFirstLine4C{ version, statusCode, std::move(statusText)});
                };
            }

            input->headers() += _sol * [&](primitives::List<www::http::Header>&& headers, bool done)
            {
                _actions.emplace_back(InputHeaders{ std::move(headers), done});
            };

            input->data() += _sol * [&](Bytes&& data, bool done)
            {
                _actions.emplace_back(InputData{ std::move(data), done});
            };

            input->done() += _sol * [&]()
            {
                _actions.emplace_back(InputDone{});
            };

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            output->setupDataProcessing(
                        www::http::message::tunable::Compression::byHeaders,
                        www::http::message::tunable::Compression::byHeaders,
                        www::http::message::tunable::Encoding::byHeaders);

            output->failed() += _sol * [&](dci::primitives::ExceptionPtr&& e)
            {
                _actions.emplace_back(OutputFailed{dci::exception::toString(e)});
            };

            output->closed() += _sol * [&]()
            {
                _actions.emplace_back(OutputClosed{});
            };

            _ios.emplace_back(IO{std::move(input), std::move(output)});
        };

        void play(int countDown = 10)
        {
            for(int cnt{countDown}; cnt>=0; --cnt)
            {
                std::size_t actionsSize = _actions.size();
                // poll::timeout(std::chrono::microseconds{1}).wait();// некоторые системы (виндоус, epoll_wait) увеличивают это время до более чем 1мс, что неприемлемо
                cmt::yield();
                if(_actions.size() != actionsSize)
                    cnt = countDown;
            }
        }

        template <class A>
        bool has() const
        {
            for(const Action& a : _actions)
                if(a.template holds<A>())
                    return true;
            return false;
        }

        template <class A>
        std::size_t count() const
        {
            std::size_t res{};
            for(const Action& a : _actions)
                if(a.template holds<A>())
                    ++res;
            return res;
        }

        template <class A>
        const A& get() const
        {
            for(const Action& a : _actions)
                if(a.template holds<A>())
                    return a.template get<A>();
            throw "no data";
        }

        template <class A, std::size_t I>
        A::template Element<I> accumulate() const
        {
            typename A::template Element<I> sum;
            for(const Action& a : _actions)
                if(a.template holds<A>())
                    sum.end().write(a.template get<A>().template get<I>());
            return sum;
        }

        template <class A>
        std::size_t pos() const
        {
            std::size_t res{};
            for(const Action& a : _actions)
            {
                if(a.template holds<A>())
                    return res;
                ++res;
            }
            throw "no data";
        }
    };

    using SpectacleForServer = Spectacle<true>;
    using SpectacleForClient = Spectacle<false>;
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
#define CHECK_IO(spectacle)                                     \
    ASSERT_GT(spectacle._actions.size(), 1);                    \
    ASSERT_EQ(spectacle._actions[0], decltype(spectacle)::Io{});

#define CHECK_FAIL_ONE(spectacle, direction, failType)                                                                              \
    ASSERT_TRUE(spectacle.has<decltype(spectacle)::direction##Failed>());                                                           \
    ASSERT_TRUE(spectacle.get<decltype(spectacle)::direction##Failed>().get<0>().starts_with("dci::idl::gen::www::http::error::" #failType "{}"));\
    ASSERT_TRUE(spectacle.has<decltype(spectacle)::direction##Failed>());                                                           \
    ASSERT_LT(spectacle.pos<decltype(spectacle)::direction##Failed>(), spectacle.pos<decltype(spectacle):: direction##Closed>());

#define CHECK_FAIL(spectacle, failType)        \
    CHECK_FAIL_ONE(spectacle, , BadInput)      \
    CHECK_FAIL_ONE(spectacle, Input, failType) \
    CHECK_FAIL_ONE(spectacle, Output, BadInput)

#define CHECK_PEERDATA(spectacle, data)                                                                         \
    ASSERT_TRUE(spectacle.has<decltype(spectacle)::PeerData>());                                                \
    ASSERT_EQ((spectacle.accumulate<decltype(spectacle)::PeerData, 0>()), data);                                \
    ASSERT_TRUE(spectacle.has<decltype(spectacle)::PeerClosed>());                                              \
    ASSERT_LT(spectacle.pos<decltype(spectacle)::PeerData>(), spectacle.pos<decltype(spectacle)::PeerClosed>());

#define CHECK_INPUTDATA(spectacle, data)                                                                            \
    ASSERT_TRUE(spectacle.has<decltype(spectacle)::InputData>());                                                   \
    ASSERT_EQ((spectacle.accumulate<decltype(spectacle)::InputData, 0>()), data);                                   \
    ASSERT_TRUE(spectacle.has<decltype(spectacle)::InputDone>());                                                   \
    ASSERT_TRUE(spectacle.has<decltype(spectacle)::InputClosed>());                                                 \
    ASSERT_LT(spectacle.pos<decltype(spectacle)::InputData>(), spectacle.pos<decltype(spectacle)::InputClosed>());

#define SERVER_PLAY_2_FAIL(req, err, resp)  \
    {                                       \
        SpectacleForServer spectacle;       \
        spectacle._peer->send(req);         \
        spectacle.play();                   \
        CHECK_IO(spectacle);                \
        CHECK_FAIL(spectacle, err);         \
        CHECK_PEERDATA(spectacle, resp);    \
    };

#define CLIENT_PLAY_2_FAIL(resp, err)       \
    {                                       \
        SpectacleForClient spectacle;       \
        spectacle.startIo();                \
        spectacle._peer->send(resp);        \
        spectacle.play();                   \
        CHECK_IO(spectacle);                \
        CHECK_FAIL(spectacle, err);         \
    };
