// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "sendBuffer.hpp"

namespace dci::module::net::stream
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SendBuffer::SendBuffer()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SendBuffer::~SendBuffer()
    {
        clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SendBuffer::push(const Bytes& data)
    {
        return push(Bytes(data));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SendBuffer::push(Bytes&& data)
    {
        _data.end().write(std::move(data));

        if(_bufsAmountMin4Enfill >= _bufsAmount)
        {
            enfillBufs();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SendBuffer::clear()
    {
        _data.clear();
        _bufsAmount = 0;
        _bufsSize = 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool SendBuffer::empty() const
    {
        return !_bufsSize;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Buf* SendBuffer::bufs()
    {
        dbgAssert(!empty());
        return &_bufs[0];
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 SendBuffer::bufsAmount() const
    {
        return _bufsAmount;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 SendBuffer::bufsSize() const
    {
        return _bufsSize;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 SendBuffer::dataSize() const
    {
        return _data.size();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SendBuffer::drop(uint32 size)
    {
        dbgAssert(_bufsSize >= size);
        dbgAssert(!empty());

        _data.begin().remove(size);
        enfillBufs();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SendBuffer::enfillBufs()
    {
        _bufsAmount = 0;
        _bufsSize = 0;
        bytes::Cursor c(_data.begin());

        while(!c.atEnd() && _bufsAmount < _bufsAmountMax)
        {
            _bufs[_bufsAmount].data() = reinterpret_cast<Buf::Data>(const_cast<byte *>(c.continuousData()));
            _bufs[_bufsAmount].len() = c.continuousDataSize();

            _bufsSize += _bufs[_bufsAmount].len();
            _bufsAmount++;
            c.advanceChunks(1);
        }
    }
}
