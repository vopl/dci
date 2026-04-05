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
#include "inputProcessResult.hpp"

namespace dci::module::www::io
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class InputImpl, class OutputImpl, bool serverMode>
    class Plexus
    {
    public:
        sbs::Owner& sol();

    public:
        Plexus(api::stream::Channel<>&& streamChannel, api::Unreliable<>::Opposite&& unreliableOpposite);
        ~Plexus();

        template <class... InputArgs, class... OutputArgs>
        void emplace(std::tuple<InputArgs...>&& inputArgs, std::tuple<OutputArgs...>&& outputArgs) requires (!serverMode);

        template <class... OutputArgs>
        void emplace(OutputArgs&&... outputArgs) requires (serverMode);

    public:
        void done(OutputImpl* output);
        void write(Bytes&& data);

        void apiWantClose(OutputImpl* output);
        void apiWantClose(InputImpl* input);

        void failed(InputImpl* input, primitives::ExceptionPtr&& e);
        void failed(OutputImpl* output, primitives::ExceptionPtr&& e);

    public:
        void close(primitives::ExceptionPtr&& e);
        bool stopped() const;

        sbs::Owner _solExternal;
        sbs::Owner _sol;

    private:
        api::stream::Channel<>      _streamChannel;
        api::Unreliable<>::Opposite _unreliableOpposite;

        using InputHolder = utils::ct::If<serverMode, InputImpl, std::deque<InputImpl>>;
        InputHolder                 _inputHolder;

        using OutputHolder = std::deque<OutputImpl>;
        OutputHolder                _outputHolder;

        bool                        _receiveStarted{};
        Bytes                       _receivedData;
        InputProcessResult          _inputProcessResult{};
        bool                        _stopped{};
    };
}

#include "plexus.ipp"
