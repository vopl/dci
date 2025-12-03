// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/cmt/task/owner.hpp>
#include <dci/sbs/wire.hpp>
#include <dci/cmt/raisable.hpp>
#include <dci/utils/intrusiveDlist.hpp>
#include <memory>
#include <atomic>

namespace dci::poll::impl
{
    class TagForAll;
    class TagForReady;

    class Awaker final
        : public utils::IntrusiveDlistElement<Awaker, TagForAll>
        , public utils::IntrusiveDlistElement<Awaker, TagForReady>
    {
        Awaker(const Awaker&) = delete;
        void operator=(const Awaker&) = delete;

    public:
        Awaker(cmt::task::Owner* wokenOwner, cmt::Raisable* raisable, bool keepLoop);
        ~Awaker();

        bool keepLoop() const;

        sbs::Signal<> woken();
        bool ready() const;
        void emitWokenIfNeed();

        void setWokenOwner(cmt::task::Owner* wokenOwner);
        void resetWokenOwner();

        void setRaisable(cmt::Raisable* raisable);
        void resetRaisable();

        void wakeup();

    private:

        struct Woken
        {
            Awaker*     _owner;
            bool        _inProgress{};
            sbs::Wire<> _wire;

            Woken(Awaker* owner) : _owner{owner} {}
        };
        using WokenPtr = std::shared_ptr<Woken>;
        WokenPtr            _woken{std::make_shared<Woken>(this)};

        cmt::task::Owner*   _wokenOwner{};
        cmt::task::Owner    _localWokenOwner{};
        cmt::Raisable*      _raisable{};

        bool                _keepLoop{true};

        std::atomic_bool    _ready{};
    };
}
