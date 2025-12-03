// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/poll/api.hpp>
#include <dci/himpl.hpp>
#include <dci/poll/implMetaInfo.hpp>
#include <dci/cmt/task/owner.hpp>
#include <dci/cmt/raisable.hpp>
#include <dci/sbs/signal.hpp>
#include "descriptor/native.hpp"
#include "descriptor/readyStateFlags.hpp"

namespace dci::poll
{
    class API_DCI_POLL Descriptor final
        : public himpl::FaceLayout<Descriptor, impl::Descriptor>
    {
        Descriptor(const Descriptor&) = delete;
        void operator=(const Descriptor&) = delete;

    public:
        using Native = descriptor::Native;
        using ReadyStateFlags = descriptor::ReadyStateFlags;

    public:
        Descriptor(Native native = {});
        Descriptor(Native native, auto&& onReady, cmt::task::Owner* readyOwner = nullptr) requires(std::invocable<decltype(onReady)&&, Native, ReadyStateFlags>);
        Descriptor(Native native, cmt::task::Owner* readyOwner);
        Descriptor(Native native, cmt::Raisable* raisable);
        ~Descriptor();

        sbs::Signal<void, Native /*native*/, ReadyStateFlags /*readyState*/> ready();
        void emitReadyIfNeed();
        void emitReady();

        void setReadyOwner(cmt::task::Owner* readyOwner);
        void resetReadyOwner();

        void setRaisable(cmt::Raisable* raisable);
        void resetRaisable();

        bool valid() const;
        std::error_code error();

        Native native() const;
        operator Native() const;

        std::error_code close();
        std::error_code attach(Native native);
        std::error_code detach();

        ReadyStateFlags readyState() const;
        void resetReadyState(ReadyStateFlags flags);
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Descriptor::Descriptor(Native native, auto&& onReady, cmt::task::Owner* readyOwner) requires(std::invocable<decltype(onReady)&&, descriptor::Native, descriptor::ReadyStateFlags>)
        : Descriptor{native, readyOwner}
    {
        this->ready() += std::forward<decltype(onReady)>(onReady);
        emitReadyIfNeed();
    }
}
