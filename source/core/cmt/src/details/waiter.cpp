// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/cmt/details/waiter.hpp>
#include "impl/details/waiter.hpp"

namespace dci::cmt::details
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Waiter::Waiter(WWLink* links, std::size_t amount)
        : himpl::FaceLayout<Waiter, impl::details::Waiter>(links, amount)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Waiter::Waiter(WWLink* links, std::size_t amount, void(*cb)(void* cbData), void* cbData)
        : himpl::FaceLayout<Waiter, impl::details::Waiter>(links, amount, cb, cbData)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Waiter::~Waiter()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::any(std::size_t* acquiredIndex)
    {
        return impl().any(acquiredIndex);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::all()
    {
        return impl().all();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::expr(ExprEvaluator ee, void* eeData, std::byte* bitsInBytes)
    {
        return impl().expr(ee, eeData, bitsInBytes);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::reset()
    {
        return impl().reset();
    }

}
