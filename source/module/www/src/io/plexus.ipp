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
#include "plexus.hpp"
#include "../channelSoftClosing.hpp"

namespace dci::module::www::io
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class InputImpl, class OutputImpl, bool serverMode>
    sbs::Owner& Plexus<InputImpl, OutputImpl, serverMode>::sol()
    {
        return _solExternal;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class InputImpl, class OutputImpl, bool serverMode>
    Plexus<InputImpl, OutputImpl, serverMode>::Plexus(api::stream::Channel<>&& streamChannel, api::Unreliable<>::Opposite&& unreliableOpposite)
        : _streamChannel{std::move(streamChannel)}
        , _unreliableOpposite{std::move(unreliableOpposite)}
    {
        if constexpr(serverMode)
            _inputHolder.setSupport(this);

        // in  send            (bytes);
        // in  startReceive    ();

        // out received        (bytes);
        _streamChannel->received() += _sol * [this](Bytes&& data)
        {
            _receivedData.end().write(std::move(data));

            auto stopReceive = [&]
            {
                if(_receiveStarted && _streamChannel)
                {
                    _receiveStarted = false;
                    _streamChannel->stopReceive();
                }
            };

            if(InputProcessResult::bad == _inputProcessResult)
            {
                stopReceive();
                return;
            }

            if constexpr(serverMode)
            {
                while(!_receivedData.empty())
                {
                    {
                        bytes::Alter receivedDataAlter = _receivedData.begin();
                        InputProcessResult inputProcessResult = _inputHolder.process(receivedDataAlter);
                        if(_stopped)
                            break;
                        _inputProcessResult = inputProcessResult;
                    }

                    switch(_inputProcessResult)
                    {
                    case InputProcessResult::needMore:
                    case InputProcessResult::done:
                        break;

                    case InputProcessResult::bad:
                        stopReceive();
                        return;
                    }
                }
            }
            else
            {
                while(!_receivedData.empty() && !_inputHolder.empty())
                {
                    {
                        bytes::Alter receivedDataAlter = _receivedData.begin();
                        InputProcessResult  inputProcessResult = _inputHolder.front().process(receivedDataAlter);
                        if(_stopped)
                            break;
                        _inputProcessResult = inputProcessResult;
                    }

                    switch(_inputProcessResult)
                    {
                    case InputProcessResult::needMore:
                        break;
                    case InputProcessResult::done:
                        _inputHolder.pop_front();
                        break;

                    case InputProcessResult::bad:
                        stopReceive();
                        return;
                    }
                }
            }
        };

        // in  stopReceive     ();

        // out failed          (exception);
        _streamChannel->failed() += _sol * [this](primitives::ExceptionPtr&& exception)
        {
            close(exception::buildInstance<api::http::error::DownstreamFailed>(std::move(exception)));
        };

        // out closed          ();
        _streamChannel->closed() += _sol * [this]()
        {
            close({});
        };

        if(serverMode || _receiveStarted)
            _streamChannel->startReceive();

        // in close();
        _unreliableOpposite->close() += _sol * [&]()
        {
            close({});
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class InputImpl, class OutputImpl, bool serverMode>
    Plexus<InputImpl, OutputImpl, serverMode>::~Plexus()
    {
        _solExternal.flush();
        _sol.flush();
        close({});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class InputImpl, class OutputImpl, bool serverMode>
    template <class... InputArgs, class... OutputArgs>
    void Plexus<InputImpl, OutputImpl, serverMode>::emplace(std::tuple<InputArgs...>&& inputArgs, std::tuple<OutputArgs...>&& outputArgs) requires (!serverMode)
    {
        {
            std::apply([&](auto&&... args)
                {
                    _inputHolder.emplace_back(this, std::forward<decltype(args)>(args)...);
                },
                std::move(inputArgs));

            if(!_receiveStarted)
            {
                _receiveStarted = true;
                _streamChannel->startReceive();
            }
        }

        {
            std::apply([&](auto&&... args)
                {
                    _outputHolder.emplace_back(this, std::forward<decltype(args)>(args)...);
                },
                std::move(outputArgs));

            _outputHolder.front().allowWrite();
        }
    }

    template <class InputImpl, class OutputImpl, bool serverMode>
    template <class... OutputArgs>
    void Plexus<InputImpl, OutputImpl, serverMode>::emplace(OutputArgs&&... outputArgs) requires (serverMode)
    {
        _outputHolder.emplace_back(this, std::forward<OutputArgs>(outputArgs)...);
        _outputHolder.front().allowWrite();
        _inputHolder.setResponse(&_outputHolder.front());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class InputImpl, class OutputImpl, bool serverMode>
    void Plexus<InputImpl, OutputImpl, serverMode>::done(OutputImpl* /*output*/)
    {
        dbgAssert(!_outputHolder.empty());

        for(;;)
        {
            if(_outputHolder.empty())
                break;

            _outputHolder.front().allowWrite();

            if(_outputHolder.empty())
                break;

            if(!_outputHolder.front().isDone())
                break;

            if(_outputHolder.front().isFail())
            {
                close({});
                break;
            }

            _outputHolder.pop_front();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class InputImpl, class OutputImpl, bool serverMode>
    void Plexus<InputImpl, OutputImpl, serverMode>::write(Bytes&& data)
    {
        _streamChannel->send(std::move(data));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class InputImpl, class OutputImpl, bool serverMode>
    void Plexus<InputImpl, OutputImpl, serverMode>::apiWantClose(OutputImpl* /*output*/)
    {
        close(ExceptionPtr{});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class InputImpl, class OutputImpl, bool serverMode>
    void Plexus<InputImpl, OutputImpl, serverMode>::apiWantClose(InputImpl* /*input*/)
    {
        close(ExceptionPtr{});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class InputImpl, class OutputImpl, bool serverMode>
    void Plexus<InputImpl, OutputImpl, serverMode>::failed(InputImpl* input, primitives::ExceptionPtr&& e)
    {
        _sol.flush();

        input->fireFailed(std::move(e));
        input->fireClosed();

        close(exception::buildInstance<api::http::error::BadInput>());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class InputImpl, class OutputImpl, bool serverMode>
    void Plexus<InputImpl, OutputImpl, serverMode>::failed(OutputImpl* output, primitives::ExceptionPtr&& e)
    {
        _sol.flush();

        output->fireFailed(std::move(e));
        output->fireClosed();

        close({});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class InputImpl, class OutputImpl, bool serverMode>
    void Plexus<InputImpl, OutputImpl, serverMode>::close(ExceptionPtr&& e)
    {
        _sol.flush();

        if(_unreliableOpposite)
        {
            if(e)
                _unreliableOpposite->failed(std::move(e));
            std::exchange(_unreliableOpposite, {})->closed();
        }

        _stopped = true;
        _receiveStarted = false;
        _receivedData.clear();

        if constexpr(serverMode)
        {
            if(e)
                _inputHolder.fireFailed(ExceptionPtr{e});
            _inputHolder.fireClosed();
        }
        else
        {
            for(InputImpl& inputImpl : _inputHolder)
            {
                if(e)
                    inputImpl.fireFailed(ExceptionPtr{e});
                inputImpl.fireClosed();
            }

            _inputHolder.clear();
        }

        for(OutputImpl& outputImpl : _outputHolder)
        {
            if(e)
                outputImpl.fireFailed(ExceptionPtr{e});
            outputImpl.fireClosed();
        }
        _outputHolder.clear();

        if(_streamChannel)
            channelSoftClosing::push(std::exchange(_streamChannel, {}));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class InputImpl, class OutputImpl, bool serverMode>
    bool Plexus<InputImpl, OutputImpl, serverMode>::stopped() const
    {
        return _stopped;
    }
}
