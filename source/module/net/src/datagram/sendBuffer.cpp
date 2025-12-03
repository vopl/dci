// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "sendBuffer.hpp"

namespace dci::module::net::datagram
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SendBuffer::SendBuffer()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SendBuffer::~SendBuffer()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SendBuffer::SourceUtilization SendBuffer::fillFrom(bytes::Cursor& src)
    {
        dbgAssert(!_bufsAmount);
        _bufsAmount = 0;

        for(;;)
        {
            if(src.atEnd())
            {
                if(_bufsAmount)
                {
                    return SourceUtilization::full;
                }

                return SourceUtilization::none;
            }

            if(_bufsAmount >= _bufsAmountMax)
            {
                return SourceUtilization::partial;
            }

            _bufs[_bufsAmount].data() = reinterpret_cast<Buf::Data>(const_cast<byte*>(src.continuousData()));
            _bufs[_bufsAmount].len() = src.continuousDataSize();

            src.advanceChunks(1);
            _bufsAmount++;
        }

        unreacheable();
        dbgWarn("unreacheable");
        return SourceUtilization::none;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Buf* SendBuffer::bufs()
    {
        return &_bufs[0];
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 SendBuffer::bufsAmount() const
    {
        return _bufsAmount;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SendBuffer::clear()
    {
        _bufsAmount = 0;
    }
}
