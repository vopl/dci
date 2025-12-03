// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include "pch.hpp"

namespace dci::module::net::stream
{
    class SendBuffer
    {
        SendBuffer(const SendBuffer&) = delete;
        void operator=(const SendBuffer&) = delete;

    public:
        SendBuffer();
        ~SendBuffer();

        void push(const Bytes& data);
        void push(Bytes&& data);

        void clear();
        bool empty() const;

        Buf* bufs();
        uint32 bufsAmount() const;
        uint32 bufsSize() const;

        uint32 dataSize() const;

        void drop(uint32 size);

    private:
        void enfillBufs();

    private:
        static constexpr uint32 _bufsAmountMin4Enfill = 16;
        static constexpr uint32 _bufsAmountMax = Buf::_maxBufs;

    private:
        Bytes   _data;

        Buf     _bufs[_bufsAmountMax];
        uint32  _bufsAmount = 0;
        uint32  _bufsSize = 0;
    };
}
