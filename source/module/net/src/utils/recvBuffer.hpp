// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include "pch.hpp"

namespace dci::module::net::utils
{
    class RecvBuffer
    {
        RecvBuffer(const RecvBuffer&) = delete;
        void operator=(const RecvBuffer&) = delete;

    public:
        RecvBuffer();
        ~RecvBuffer();

        void limitDataSize(uint32 maxDataSize);
        void unlimitDataSize();

        Buf* bufs();
        uint32 bufsAmount();
        uint32 bufSize();
        uint32 dataSize();

        Bytes detach(uint32 size);

    private:
        void renewFront(uint32 bufsAmount);

    private:
        static constexpr uint32 _bufSize = bytes::Chunk::bufferSize();
        static constexpr uint32 _maxBufsAmount = Buf::_maxBufs;
        static constexpr uint32 _maxDataSize = _bufSize * _maxBufsAmount;

    private:
        bytes::Chunk*   _chunks[_maxBufsAmount];
        Buf             _bufs[_maxBufsAmount];

        uint32          _bufsAmount{_maxBufsAmount};
        uint32          _dataSize{_maxDataSize};
    };
}
