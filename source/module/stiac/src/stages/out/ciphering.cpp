// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "ciphering.hpp"
#include "../../protocol.hpp"

namespace dci::module::stiac::stages::out
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Ciphering::urgent(MessageType mt, const void* data, uint32 dataSize)
    {
        switch(mt)
        {
        case MessageType::fakeNull:
            dbgAssert(data);
            dbgAssert(dataSize);
            _output.end().write(data, dataSize);
            _protocol->linkHasOutput(this);
            return;

        case MessageType::protocolMarker:
            {
                dbgAssert(dataSize < 0x10000);

                Bytes msg;
                {
                    bytes::Alter a(msg.end());

                    std::array<uint8, _genericHeaderSize + _payloadHeaderSize> header;
                    static_assert(1 == _genericHeaderSize);
                    static_assert(2 == _payloadHeaderSize);
                    header[0] = static_cast<uint8>(MessageType::protocolMarker);
                    header[1] = dataSize & 0xff;
                    header[2] = (dataSize>>8) & 0xff;

                    a.write(header.data(), header.size());

                    a.write(data, dataSize);
                }
                encrypt(std::move(msg), true);
            }
            return;

        case MessageType::ekey:
        case MessageType::skey:
        case MessageType::keyApplied:
            {
                Bytes msg;
                {
                    bytes::Alter a(msg.end());

                    std::array<uint8, _genericHeaderSize> header;
                    static_assert(1 == _genericHeaderSize);
                    header[0] = static_cast<uint8>(mt);
                    a.write(header.data(), header.size());

                    a.write(data, dataSize);
                }
                encrypt(std::move(msg), true);
            }
            return;

        default:
            dbgWarn("never here");
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Ciphering::allowPayload()
    {
        _payloadAllowed = true;

        if(!_payload.empty())
        {
            flushPayload();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint16 Ciphering::getWantedEmptyPrefix() const
    {
        //приплюсовать 3 байта на заголовок
        return _wantedEmptyPrefix + _genericHeaderSize + _payloadHeaderSize;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Ciphering::input(Bytes&& payload)
    {
        dbgAssert(!payload.empty());

        _payload.end().write(std::move(payload));

        if(!_payloadAllowed)
        {
            _hs->outputHasDisallowedPayload();
        }

        if(_payloadAllowed)
        {
            flushPayload();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes Ciphering::flushOutput()
    {
        return std::move(_output);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Ciphering::flushPayload()
    {
        dbgAssert(!_payload.empty());

        constexpr uint32 maxHeaderBodySize = 0x10000 - _macSize;
        constexpr uint32 maxBodySize = maxHeaderBodySize - _genericHeaderSize - _payloadHeaderSize;

        uint32 trafficSize = _payload.size();

        while(_payload.size() > maxBodySize)
        {
            std::array<uint8, _genericHeaderSize + _payloadHeaderSize> header;
            static_assert(1 == _genericHeaderSize);
            static_assert(2 == _payloadHeaderSize);
            header[0] = static_cast<uint8>(MessageType::payloadChunk);
            header[1] = maxBodySize & 0xff;
            header[2] = (maxBodySize>>8) & 0xff;

            bytes::Alter src(_payload.begin());
            Bytes chunk;

            if(src.continuousDataOffset() >= sizeof(header))
            {
                src.advance(-int32(header.size()));
                src.write(header.data(), header.size());
                src.advance(-int32(header.size()));
                src.removeTo(chunk, maxHeaderBodySize);
            }
            else
            {
                bytes::Alter dst(chunk.begin());
                dst.write(header.data(), header.size());
                src.removeTo(dst, maxBodySize);
            }

            encrypt(std::move(chunk), false);
        }

        {
            uint32 bodySize = _payload.size();
            dbgAssert(bodySize <= maxBodySize);

            std::array<uint8, _genericHeaderSize + _payloadHeaderSize> header;
            static_assert(1 == _genericHeaderSize);
            static_assert(2 == _payloadHeaderSize);
            header[0] = static_cast<uint8>(MessageType::payloadLastChunk);
            header[1] = bodySize & 0xff;
            header[2] = (bodySize>>8) & 0xff;

            bytes::Alter a(_payload.begin());
            a.advance(-int32(header.size()));
            a.write(header.data(), header.size());
            encrypt(std::move(_payload), false);
        }

        _hs->outputTraffic(trafficSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Ciphering::encrypt(Bytes&& chunk, bool mixCiphertext2Hash)
    {
        {
            //начать новое сообщение
            messageStart();

            if(mixCiphertext2Hash)
            {
                mixHashStart();
            }

            bytes::Alter alter(chunk.begin());

            //шифровать тело
            while(!alter.atEnd())
            {
                messageEncipher(alter.continuousData4Write(), alter.continuousDataSize());

                if(mixCiphertext2Hash)
                {
                    mixHashUpdate(alter.continuousData4Write(), alter.continuousDataSize());
                }

                alter.advanceChunks(1);
            }

            //дописать мак
            uint8 macOut[_macSize];
            messageEncipherFinish(macOut);

            if(mixCiphertext2Hash)
            {
                mixHashUpdate(macOut, _macSize);
                mixHashFinish();
            }

            alter.write(macOut, _macSize);
        }

        //лить на выход
        _output.end().write(std::move(chunk));
        _protocol->linkHasOutput(this);
    }
}
