// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/cmt/future.hpp>
#include <dci/mm/heap/allocable.hpp>
#include <dci/sbs/owner.hpp>
#include "impl.hpp"
#include "base.hpp"
#include "source.hpp"

namespace dci::stiac::link
{
    template <class T>
    class Impl<cmt::Future<T>> final
        : public Base
        , public mm::heap::Allocable<Impl<cmt::Future<T>>>
    {
    public:
        Impl(const cmt::Future<T>& future);
        Impl(cmt::Future<T>&& future);

        void initialize(Hub4Link* hub, Id id) override;
        void input(Source& source) override;
        void destroy() override;

    private:
        sbs::Owner          _sbsOwner;
        cmt::Future<T>    _future;
    };

}
