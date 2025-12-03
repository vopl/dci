// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "base.hpp"
#include "inputProcessResult.hpp"

namespace dci::module::www::io
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api, bool serverMode>
    class InputBase : public Base<Support, Impl, Api>
    {
    public:
        InputBase() requires (serverMode);
        InputBase(Support* support, Api&& api) requires (!serverMode);
        ~InputBase();

    public:
        void setSupport(Support* support) requires (serverMode);

    public:
        void fireFailed(ExceptionPtr&&);
        void fireClosed(bool andReset = true);

    public:
        InputProcessResult process(bytes::Alter& data) = delete;

    public:
        bool _emitDataDoneOnClose{};
    };
}

#include "inputBase.ipp"
