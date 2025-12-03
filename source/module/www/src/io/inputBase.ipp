// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "inputBase.hpp"

namespace dci::module::www::io
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api, bool serverMode>
    InputBase<Support, Impl, Api, serverMode>::InputBase() requires (serverMode)
        : Base<Support, Impl, Api>{}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api, bool serverMode>
    InputBase<Support, Impl, Api, serverMode>::InputBase(Support* support, Api&& api) requires (!serverMode)
        : Base<Support, Impl, Api>{support, std::move(api)}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api, bool serverMode>
    InputBase<Support, Impl, Api, serverMode>::~InputBase()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api, bool serverMode>
    void InputBase<Support, Impl, Api, serverMode>::fireFailed(primitives::ExceptionPtr&& e)
    {
        _emitDataDoneOnClose = false;
        return Base<Support, Impl, Api>::fireFailed(std::move(e));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api, bool serverMode>
    void InputBase<Support, Impl, Api, serverMode>::fireClosed(bool andReset)
    {
        if(_emitDataDoneOnClose && this->_api)
        {
            this->_api->data(Bytes{}, true);
            this->_api->done();
        }

        return Base<Support, Impl, Api>::fireClosed(andReset);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api, bool serverMode>
    void InputBase<Support, Impl, Api, serverMode>::setSupport(Support* support) requires (serverMode)
    {
        Base<Support, Impl, Api>::setSupport(support);
    }
}
