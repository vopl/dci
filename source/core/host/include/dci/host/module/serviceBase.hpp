// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/mm/heap/allocable.hpp>
#include <dci/sbs/owner.hpp>

namespace dci::host::module
{
    template <class Srv>
    class ServiceBase
        : public mm::heap::Allocable<Srv>
    {
    public:
        ~ServiceBase()
        {
            _sol.flush();
        }

        sbs::Owner& serviceSol()
        {
            return _sol;
        }

    private:
        sbs::Owner _sol;
    };
}
