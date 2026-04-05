/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "ciphering.hpp"
#include "../../protocol.hpp"

namespace dci::module::stiac::stages::in
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Ciphering::input(Bytes&& data)
    {
        if(State::bad == _state)
        {
            dbgWarn("input passed to bad cipher");
            return;
        }

        if(!_keySetted)
        {
            bytes::Alter a(data.begin());
            _hs->inputComing(MessageType::fakeNull, a);

            if(a.atEnd())
            {
                return;
            }
        }

        dbgAssert(!data.empty());

        _input.end().write(std::move(data));

        for(;;)
        {
            switch(_state)
            {
            case State::awaitType:
                if(!readType()) return;
                break;

            case State::awaitSize:
                if(!readSize()) return;
                break;

            case State::awaitPayload:
                if(!readPayload()) return;
                break;

            case State::bad:
                return;
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Ciphering::readType()
    {
        if(_input.size() < _genericHeaderSize)
        {
            //не хватает данных, ждем
            return false;
        }

        //начать новое сообщение
        messageStart();

        //вычитать заголовок и переработать его
        static_assert(1 == _genericHeaderSize);
        std::array<uint8, _genericHeaderSize> genericHeader;
        std::array<uint8, _genericHeaderSize> genericHeaderCiphertext;
        _input.begin().removeTo(genericHeader.data(), genericHeader.size());
        genericHeaderCiphertext = genericHeader;

        messageDecipher(genericHeader.data(), genericHeader.size());

        _messageType = static_cast<MessageType>(genericHeader[0]);

        switch(_messageType)
        {
        case MessageType::payloadChunk:
        case MessageType::payloadLastChunk:
            _messageMix2Hash = false;
            break;

        default:
            _messageMix2Hash = true;
            mixHashStart();
            mixHashUpdate(genericHeaderCiphertext.data(), genericHeaderCiphertext.size());
            break;
        }

        switch(_messageType)
        {
        case MessageType::payloadChunk:
        case MessageType::payloadLastChunk:
        case MessageType::protocolMarker:
            _state = State::awaitSize;
            break;

        case MessageType::ekey:
        case MessageType::skey:
            _messageSize = _keySize;
            _state = State::awaitPayload;
            break;

        case MessageType::keyApplied:
            _messageSize = 0;
            _state = State::awaitPayload;
            break;

        default:
            _state = State::bad;
            _protocol->decipheringFail("bad data from remote");
            return true;
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Ciphering::readSize()
    {
        if(_input.size() < _payloadHeaderSize)
        {
            //не хватает данных, ждем
            return false;
        }

        static_assert(2 == _payloadHeaderSize);
        std::array<uint8, _payloadHeaderSize> payloadHeader;

        dbgAssert(_input.size() >= _payloadHeaderSize);
        uint32 removed = _input.begin().removeTo(payloadHeader.data(), payloadHeader.size());
        dbgAssert(removed == _payloadHeaderSize);
        (void)removed;

        if(_messageMix2Hash)
        {
            mixHashUpdate(payloadHeader.data(), payloadHeader.size());
        }

        messageDecipher(payloadHeader.data(), payloadHeader.size());

        _messageSize = static_cast<uint32>(payloadHeader[0] | (payloadHeader[1]<<8));

        _state = State::awaitPayload;
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Ciphering::readPayload()
    {
        if(_input.size() < _messageSize + _macSize)
        {
            //не хватает данных тела и контрольной суммы, ждем
            return false;
        }

        Bytes chunk;
        bytes::Alter src(_input.begin());

        //вычитать шифрованное тело
        src.removeTo(chunk, _messageSize);
        dbgAssert(chunk.size() == _messageSize);

        //расшифровка тела
        {
            bytes::Alter c(chunk.begin());
            while(!c.atEnd())
            {
                if(_messageMix2Hash)
                {
                    mixHashUpdate(c.continuousData4Write(), c.continuousDataSize());
                }

                messageDecipher(c.continuousData4Write(), c.continuousDataSize());
                c.advanceChunks(1);
            }
        }

        //валидация
        uint8 macIn[_macSize];
        src.removeTo(macIn, _macSize);

        if(!messageDecipherFinish(macIn))
        {
            _state = State::bad;
            _protocol->integrityViolation(this);
            return true;
        }

        if(_messageMix2Hash)
        {
            mixHashUpdate(macIn, _macSize);
            mixHashFinish();
        }

        _hs->someInputMessageDeciphered();

        MessageType messageType = _messageType;

        _state = State::awaitType;
        _messageType = MessageType::fakeNull;
        _messageSize = 0;
        _messageMix2Hash = false;

        switch(messageType)
        {
        case MessageType::payloadChunk:
            _payload.end().write(std::move(chunk));
            break;

        case MessageType::payloadLastChunk:
            {
                _payload.end().write(std::move(chunk));
                uint32 trafficSize = _payload.size();
                accumulateOutput(std::move(_payload));
                _hs->inputTraffic(trafficSize);
            }
            break;

        case MessageType::protocolMarker:
        case MessageType::ekey:
        case MessageType::skey:
            {
                bytes::Alter a(chunk.begin());
                _hs->inputComing(messageType, a);
                dbgAssert(0 == chunk.size());
            }
            break;

        case MessageType::keyApplied:
            {
                bytes::Alter fakeAlter;
                _hs->inputComing(messageType, fakeAlter);
            }
            break;

        default:
            _state = State::bad;
            _protocol->decipheringFail("bad data from remote");
            return true;
        }

        return true;
    }
}
