// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "impl/descriptor.hpp"
#include <dci/poll/descriptor.hpp>

namespace dci::poll
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Descriptor::Descriptor(Native native)
        : FaceLayout{native, nullptr, nullptr}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Descriptor::Descriptor(Native native, cmt::task::Owner* actOwner)
        : FaceLayout{native, actOwner, nullptr}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Descriptor::Descriptor(Native native, cmt::Raisable* raisable)
        : FaceLayout{native, nullptr, raisable}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Descriptor::~Descriptor()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, descriptor::Native /*native*/, descriptor::ReadyStateFlags /*readyState*/> Descriptor::ready()
    {
        return impl().ready();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Descriptor::emitReadyIfNeed()
    {
        return impl().emitReadyIfNeed();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Descriptor::emitReady()
    {
        return impl().emitReady();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Descriptor::setReadyOwner(cmt::task::Owner* actOwner)
    {
        return impl().setReadyOwner(actOwner);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Descriptor::resetReadyOwner()
    {
        return impl().resetReadyOwner();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Descriptor::setRaisable(cmt::Raisable* raisable)
    {
        return impl().setRaisable(raisable);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Descriptor::resetRaisable()
    {
        return impl().resetRaisable();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Descriptor::valid() const
    {
        return impl().valid();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Descriptor::error()
    {
        return impl().error();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    descriptor::Native Descriptor::native() const
    {
        return impl().native();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Descriptor::operator Native() const
    {
        return impl().native();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Descriptor::close()
    {
        return impl().close();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Descriptor::attach(Native native)
    {
        return impl().attach(native);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Descriptor::detach()
    {
        return impl().detach();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    descriptor::ReadyStateFlags Descriptor::readyState() const
    {
        return impl().readyState();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Descriptor::resetReadyState(ReadyStateFlags flags)
    {
        return impl().resetReadyState(flags);
    }
}

