// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/cmt/promise.hpp>
#include <dci/mm/heap/allocable.hpp>
#include <dci/sbs/owner.hpp>
#include "impl.hpp"
#include "base.hpp"
#include "source.hpp"

namespace dci::stiac::link
{
    template <class T>
    class Impl<cmt::Promise<T>> final
        : public Base
        , public mm::heap::Allocable<Impl<cmt::Promise<T>>>
    {
    public:
        Impl();
        Impl(cmt::Promise<T>&& promise);

        void initialize(Hub4Link* hub, Id id) override;
        void input(Source& source) override;
        void destroy() override;

    public:
        cmt::Future<T> target();

    private:
        sbs::Owner          _sbsOwner;
        cmt::Promise<T>   _promise;
    };

}
