// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::stiac
{
    class Protocol;
}

namespace dci::module::stiac::stages
{
    class Base
    {
    public:
        Base(Protocol* protocol);
        virtual ~Base();

        virtual bool initialize();

        void setIndexInChain(std::size_t index);
        std::size_t getIndexInChain() const;

        void setWantedEmptyPrefix(uint16 size);
        virtual uint16 getWantedEmptyPrefix() const;

        virtual void input(Bytes&& msg);

        virtual bool hasOutput() const;
        virtual Bytes flushOutput();

        Bytes& outputBuffer();

    protected:
        void accumulateOutput(Bytes&& msg);

    protected:
        Protocol* _protocol;
        std::size_t _indexInChain = ~std::size_t();

        uint16 _wantedEmptyPrefix = 0;

        Bytes _output;
    };
}
