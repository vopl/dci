// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/poll/awaker.hpp>
#include "impl/awaker.hpp"

namespace dci::poll
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Awaker::Awaker(bool keepLoop)
        : himpl::FaceLayout<Awaker, impl::Awaker>{nullptr, nullptr, keepLoop}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Awaker::Awaker(cmt::task::Owner* wokenOwner, bool keepLoop)
        : himpl::FaceLayout<Awaker, impl::Awaker>{wokenOwner, nullptr, keepLoop}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Awaker::Awaker(cmt::Raisable* raisable, bool keepLoop)
        : himpl::FaceLayout<Awaker, impl::Awaker>{nullptr, raisable, keepLoop}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Awaker::~Awaker()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Awaker::keepLoop() const
    {
        return impl().keepLoop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<> Awaker::woken()
    {
        return impl().woken();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Awaker::setWokenOwner(cmt::task::Owner* wokenOwner)
    {
        return impl().setWokenOwner(wokenOwner);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Awaker::resetWokenOwner()
    {
        return impl().resetWokenOwner();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Awaker::setRaisable(cmt::Raisable* raisable)
    {
        return impl().setRaisable(raisable);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Awaker::resetRaisable()
    {
        return impl().resetRaisable();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Awaker::wakeup()
    {
        return impl().wakeup();
    }
}
