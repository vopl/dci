// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../implMetaInfo.hpp"

namespace dci::himpl::details
{

    template <class TImpl, bool direct>
    struct ImplDestructionExecutor
    {
        static void exec(TImpl* pimpl)
            noexcept
            requires(ImplMetaInfo<TImpl>::_hasDestructorCaller)
        {
            //std::cout<<"try "<<typeid(TImpl).name()<<std::endl;
            pimpl->tryDestruction(pimpl);
        }
    };



    template <class TImpl>
    struct ImplDestructionExecutor<TImpl, true>
    {
        static void exec(TImpl* pimpl)
            noexcept(ImplMetaInfo<TImpl>::_hasNothrowDestructor)
            requires(ImplMetaInfo<TImpl>::_hasDestructor)
        {
            //std::cout<<"direct "<<typeid(TImpl).name()<<std::endl;
            pimpl->~TImpl();
        }
    };

}
