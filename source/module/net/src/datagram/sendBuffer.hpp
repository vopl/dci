// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include "pch.hpp"

namespace dci::module::net::datagram
{
    class SendBuffer
    {
        SendBuffer(const SendBuffer&) = delete;
        void operator=(const SendBuffer&) = delete;

    public:
        SendBuffer();
        ~SendBuffer();

        enum class SourceUtilization
        {
            none,
            partial,
            full,
        };

        SourceUtilization fillFrom(bytes::Cursor& src);

        Buf* bufs();
        uint32 bufsAmount() const;

        void clear();

    private:
        static constexpr uint32 _bufsAmountMax = Buf::_maxBufs;

    private:
        Buf     _bufs[_bufsAmountMax];
        uint32  _bufsAmount = 0;
    };
}
