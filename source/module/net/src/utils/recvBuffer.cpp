// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "recvBuffer.hpp"

namespace dci::module::net::utils
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    RecvBuffer::RecvBuffer()
    {
        renewFront(_maxBufsAmount);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    RecvBuffer::~RecvBuffer()
    {
        for(bytes::Chunk* c : _chunks)
        {
            delete c;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void RecvBuffer::limitDataSize(uint32 maxDataSize)
    {
        if(_bufsAmount < _maxBufsAmount)
        {
            _bufs[_bufsAmount-1].len() = _bufSize;
        }

        if(!maxDataSize || maxDataSize >= _maxDataSize)
        {
            _bufsAmount = _maxBufsAmount;
            _dataSize = _maxDataSize;
            return;
        }

        _bufsAmount = (maxDataSize + _bufSize - 1) / _bufSize;
        _dataSize = maxDataSize;

        _bufs[_bufsAmount-1].len() = maxDataSize % _bufSize;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void RecvBuffer::unlimitDataSize()
    {
        limitDataSize(_maxDataSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Buf* RecvBuffer::bufs()
    {
        return &_bufs[0];
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 RecvBuffer::bufsAmount()
    {
        return _bufsAmount;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 RecvBuffer::bufSize()
    {
        return _bufSize;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 RecvBuffer::dataSize()
    {
        return _dataSize;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes RecvBuffer::detach(uint32 size)
    {
        dbgAssert(size > 0);

        if(size >= _maxDataSize)
        {
            dci::utils::AtScopeExit after{[&]()
            {
                renewFront(_bufsAmount);
            }};

            return Bytes{_chunks[0], _chunks[_bufsAmount-1], _maxDataSize};
        }

        uint32 bufsAmount = size / _bufSize;
        dbgAssert(bufsAmount <= _maxBufsAmount);
        if(bufsAmount * _bufSize < size)
        {
            _chunks[bufsAmount]->setEnd(static_cast<uint16>(size - bufsAmount * _bufSize));
            bufsAmount++;
        }
        _chunks[bufsAmount-1]->setNext(nullptr);

        dci::utils::AtScopeExit after{[&]()
        {
            renewFront(bufsAmount);
        }};

        return Bytes{_chunks[0], _chunks[bufsAmount-1], size};
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void RecvBuffer::renewFront(uint32 bufsAmount)
    {
        dbgAssert(bufsAmount <= _maxBufsAmount);
        [[assume(bufsAmount <= _maxBufsAmount)]];

        if(_dataSize < _maxDataSize)
        {
            if(bufsAmount < _bufsAmount)
            {
                _bufs[_bufsAmount-1].len() = _bufSize;
            }

            _bufsAmount = _maxBufsAmount;
            _dataSize = _maxDataSize;
        }

        dbgAssert(bufsAmount);

        bytes::Chunk* prev = nullptr;

        {
            bytes::Chunk *&cur = _chunks[0];
            cur = new bytes::Chunk{nullptr, nullptr, 0, _bufSize};

            _bufs[0].data() = reinterpret_cast<Buf::Data>(cur->data());
            _bufs[0].len() = _bufSize;

            prev = cur;
        }

        for(uint32 i(1); i<bufsAmount; ++i)
        {
            bytes::Chunk *&cur = _chunks[i];
            cur = new bytes::Chunk{nullptr, prev, 0, _bufSize};

            _bufs[i].data() = reinterpret_cast<Buf::Data>(cur->data());
            _bufs[i].len() = _bufSize;

            prev->setNext(cur);
            prev = cur;
        }

        if(bufsAmount < _bufsAmount)
        {
            dbgAssert(bufsAmount < _maxBufsAmount);
            [[assume(bufsAmount < _maxBufsAmount)]];

            bytes::Chunk *&cur = _chunks[bufsAmount];

            prev->setNext(cur);
            cur->setPrev(prev);
        }
    }

    RecvBuffer recvBuffer;
}
