// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "base.hpp"

namespace dci::module::www::io
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api>
    class OutputBase : public Base<Support, Impl, Api>
    {
    protected:
        OutputBase(Support* support, Api&& api);
        ~OutputBase();

    public:
        bool isFail();
        bool isDone();
        void allowWrite();

    protected:
        void flushBuffer();
        void apiDone();
        void fail();

    protected:
        Bytes    _buffer;
        bool     _writeAllowed{};
        bool     _apiDone{};
        bool     _fail{};
    };
}

#include "outputBase.ipp"
