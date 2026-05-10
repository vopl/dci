/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/sbs/owner.hpp>
#include <dci/sbs/signal/adapter.hpp>

#include "../contract/id.hpp"
#include "../contract/lid.hpp"
#include "../contract/hmDescriptorBase.hpp"
#include "../contract/hmDescriptor.hpp"
#include "../contract/mdDescriptor.hpp"
#include "../contract/mdState.hpp"
#include "../contract/wiresBundle.hpp"

#include "side.hpp"
#include "initializer.hpp"
#include "generic.hpp"
#include "methods.hpp"
#include "methodsBase.hpp"

#include <compare>

namespace dci::idl::interface
{
    template <template <Side> class C, Side s>
    class ImplBase
        : private MethodsBase<C, s>
    {
    public:
        ImplBase();
        ~ImplBase();

        explicit ImplBase(Initializer);

    protected:
        explicit ImplBase(contract::MdState* mdcState);

    public:
        template <bool strong>
        ImplBase(const Generic<strong>& other);

        template <bool strong>
        ImplBase(Generic<strong>&& other);

        template <bool strong>
        C<s>& operator=(const Generic<strong>& other);

        template <bool strong>
        C<s>& operator=(Generic<strong>&& other);

    public:
        template <bool strong>
        bool operator== (const Generic<strong>& other) const;

        template <bool strong>
        bool operator!= (const Generic<strong>& other) const;

        template <bool strong>
        std::strong_ordering operator<=>(const Generic<strong>& other) const;

    public:
        ImplBase(const ImplBase& other);
        ImplBase(ImplBase&& other);

        C<s>& operator=(const ImplBase& other);
        C<s>& operator=(ImplBase&& other);

    public:
        template <template <Side> class O, Side oSide>
        ImplBase(const O<oSide>& other) requires(contract::MdDescriptor<O>::_declared);

        template <template <Side> class O, Side oSide>
        ImplBase(O<oSide>&& other) requires(contract::MdDescriptor<O>::_declared);

        template <template <Side> class O, Side oSide>
        C<s>& operator=(const O<oSide>& other) requires(contract::MdDescriptor<O>::_declared);

        template <template <Side> class O, Side oSide>
        C<s>& operator=(O<oSide>&& other) requires(contract::MdDescriptor<O>::_declared);

    public:
        template <template <Side> class O, Side oSide>
        bool operator==(const O<oSide>& o) const requires(contract::MdDescriptor<O>::_declared);

        template <template <Side> class O, Side oSide>
        bool operator!=(const O<oSide>& o) const requires(contract::MdDescriptor<O>::_declared);

        template <template <Side> class O, Side oSide>
        std::strong_ordering operator<=>(const O<oSide>& o) const requires(contract::MdDescriptor<O>::_declared);

    public:
        explicit operator bool() const;
        bool operator!() const;

    public:
        using Internal = ImplBase;
        using Opposite = C<invert(s)>;

        void reset();
        void init();
        Opposite init2();
        Opposite opposite() const;
        Generic<false> weak() const;

        static const contract::Id& cId();
        static const contract::Lid& cLid();

        static Side side();

        static Id id();
        static Lid lid();

        const contract::Id& mdcId() const;
        const contract::Lid& mdcLid() const;

        Side mdSide() const;

        Lid mdLid() const;
        Id mdId() const;

        bool isInvolved() const;

        sbs::Signal<void, bool> involvedChanged() const;
        auto involved() const;
        auto uninvolved() const;

        const Methods<C, s>* methods() const;
        const Methods<C, s>* operator->() const;

    protected:
        const contract::HmDescriptorBase*   hmcDescriptor() const;
        const contract::HmDescriptorBase*&  hmcDescriptor();
        contract::MdState*                  mdcState() const;
        contract::MdState*&                 mdcState();

        template <template <Side> class B>
        contract::WiresBundle<B>& getWiresBundle() const;

    private:
        void addRef();
        void release();

    private:
        template <template <Side> class, Side>
        friend class ImplBase;

        template <bool>
        friend class Generic;
    };



    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    ImplBase<C, s>::ImplBase()
    {
        hmcDescriptor() = {};
        mdcState() = {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    ImplBase<C, s>::~ImplBase()
    {
        release();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    ImplBase<C, s>::ImplBase(Initializer)
    {
        hmcDescriptor() = &contract::hmDescriptor<C, C>;
        mdcState() = contract::MdDescriptor<C>::createMds();
        addRef();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    ImplBase<C, s>::ImplBase(contract::MdState* mdcState_)
    {
        hmcDescriptor() = mdcState_ ? mdcState_->template resolveHmd<C>() : nullptr;
        mdcState() = hmcDescriptor() ? mdcState_ : nullptr;
        addRef();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    template <bool strong>
    ImplBase<C, s>::ImplBase(const Generic<strong>& other)
    {
        hmcDescriptor() = other.mdcState() ? other.mdcState()->template resolveHmd<C>() : nullptr;
        mdcState() = hmcDescriptor() ? other.mdcState() : nullptr;
        addRef();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    template <bool strong>
    ImplBase<C, s>::ImplBase(Generic<strong>&& other)
    {
        hmcDescriptor() = other.mdcState() ? other.mdcState()->template resolveHmd<C>() : nullptr;
        mdcState() = hmcDescriptor() ? other.mdcState() : nullptr;
        addRef();
        other.release();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    template <bool strong>
    C<s>& ImplBase<C, s>::operator=(const Generic<strong>& other)
    {
        if(other.mdcState() != mdcState())
        {
            release();

            if(other.mdcState())
            {
                hmcDescriptor() = other.mdcState()->template resolveHmd<C>();
                if(hmcDescriptor())
                {
                    mdcState() = other.mdcState();
                    addRef();
                }
            }
        }

        return static_cast<C<s>&>(*this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    template <bool strong>
    C<s>& ImplBase<C, s>::operator=(Generic<strong>&& other)
    {
        if(other.mdcState() != mdcState())
        {
            release();

            if(other.mdcState())
            {
                hmcDescriptor() = other.mdcState()->template resolveHmd<C>();
                if(hmcDescriptor())
                {
                    mdcState() = other.mdcState();
                    addRef();
                }

                other.release();
            }
        }

        return static_cast<C<s>&>(*this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    template <bool strong>
    bool ImplBase<C, s>::operator==(const Generic<strong>& other) const
    {
        return mdcState() == other.mdcState();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    template <bool strong>
    bool ImplBase<C, s>::operator!=(const Generic<strong>& other) const
    {
        return !operator==(other);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    template <bool strong>
    std::strong_ordering ImplBase<C, s>::operator<=> (const Generic<strong>& other) const
    {
        return mdcState() <=> other.mdcState();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    ImplBase<C, s>::ImplBase(const ImplBase& other)
    {
        hmcDescriptor() = other.hmcDescriptor();
        mdcState() = other.mdcState();
        addRef();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    ImplBase<C, s>::ImplBase(ImplBase&& other)
    {
        hmcDescriptor() = other.hmcDescriptor();
        mdcState() = other.mdcState();
        other.mdcState() = nullptr;
        other.hmcDescriptor() = nullptr;
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    C<s>& ImplBase<C, s>::operator=(const ImplBase& other)
    {
        if(other.mdcState() != mdcState())
        {
            release();

            if(other.mdcState())
            {
                mdcState() = other.mdcState();
                hmcDescriptor() = other.hmcDescriptor();
                addRef();
            }
        }

        return static_cast<C<s>&>(*this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    C<s>& ImplBase<C, s>::operator=(ImplBase&& other)
    {
        if(other.mdcState() != mdcState())
        {
            release();

            if(other.mdcState())
            {
                mdcState() = other.mdcState();
                hmcDescriptor() = other.hmcDescriptor();

                other.mdcState() = nullptr;
                other.hmcDescriptor() = nullptr;
            }
        }

        return static_cast<C<s>&>(*this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    template <template <Side> class O, Side oSide>
    ImplBase<C, s>::ImplBase(const O<oSide>& other) requires(contract::MdDescriptor<O>::_declared)
    {
        hmcDescriptor() = other.mdcState() ? other.mdcState()->template resolveHmd<C, O>(other.hmcDescriptor()) : nullptr;
        mdcState() = hmcDescriptor() ? other.mdcState() : nullptr;
        addRef();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    template <template <Side> class O, Side oSide>
    ImplBase<C, s>::ImplBase(O<oSide>&& other) requires(contract::MdDescriptor<O>::_declared)
    {
        hmcDescriptor() = other.mdcState() ? other.mdcState()->template resolveHmd<C, O>(other.hmcDescriptor()) : nullptr;
        mdcState() = hmcDescriptor() ? other.mdcState() : nullptr;
        addRef();
        other.release();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    template <template <Side> class O, Side oSide>
    C<s>& ImplBase<C, s>::operator=(const O<oSide>& other) requires(contract::MdDescriptor<O>::_declared)
    {
        if(other.mdcState() != mdcState())
        {
            release();

            if(other.mdcState())
            {
                hmcDescriptor() = other.mdcState()->template resolveHmd<C, O>(other.hmcDescriptor());
                if(hmcDescriptor())
                {
                    mdcState() = other.mdcState();
                    addRef();
                }
            }
        }

        return static_cast<C<s>&>(*this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    template <template <Side> class O, Side oSide>
    C<s>& ImplBase<C, s>::operator=(O<oSide>&& other) requires(contract::MdDescriptor<O>::_declared)
    {
        if(other.mdcState() != mdcState())
        {
            release();

            if(other.mdcState())
            {
                hmcDescriptor() = other.mdcState()->template resolveHmd<C, O>(other.hmcDescriptor());
                if(hmcDescriptor())
                {
                    mdcState() = other.mdcState();
                    addRef();
                }

                other.release();
            }
        }

        return static_cast<C<s>&>(*this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    template <template <Side> class O, Side oSide>
    bool ImplBase<C, s>::operator==(const O<oSide>& other) const requires(contract::MdDescriptor<O>::_declared)
    {
        return mdcState() == other.mdcState();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    template <template <Side> class O, Side oSide>
    bool ImplBase<C, s>::operator!=(const O<oSide>& other) const requires(contract::MdDescriptor<O>::_declared)
    {
        return !operator==(other);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    template <template <Side> class O, Side oSide>
    std::strong_ordering ImplBase<C, s>::operator<=>(const O<oSide>& other) const requires(contract::MdDescriptor<O>::_declared)
    {
        return mdcState() <=> other.mdcState();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    ImplBase<C, s>::operator bool() const
    {
        dbgAssert(!mdcState() || hmcDescriptor());
        return !!mdcState();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    bool ImplBase<C, s>::operator!() const
    {
        return !operator bool();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    void ImplBase<C, s>::reset()
    {
        release();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    void ImplBase<C, s>::init()
    {
        dbgAssert(!mdcState());

        hmcDescriptor() = &contract::hmDescriptor<C, C>;
        mdcState() = contract::MdDescriptor<C>::createMds();
        addRef();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    typename ImplBase<C, s>::Opposite ImplBase<C, s>::init2()
    {
        init();
        return opposite();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    typename ImplBase<C, s>::Opposite ImplBase<C, s>::opposite() const
    {
        return C<invert(s)>{static_cast<const C<s>&>(*this)};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    Generic<false> ImplBase<C, s>::weak() const
    {
        return Generic<false>{static_cast<const C<s>&>(*this)};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    const contract::Id& ImplBase<C, s>::cId()
    {
        return contract::MdDescriptor<C>::_id;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    const contract::Lid& ImplBase<C, s>::cLid()
    {
        return contract::mdDescriptor<C>._lid;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    Side ImplBase<C, s>::side()
    {
        return s;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    interface::Id ImplBase<C, s>::id()
    {
        return interface::Id {cId(), side()};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    interface::Lid ImplBase<C, s>::lid()
    {
        return interface::Lid {cLid(), side()};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    const contract::Id& ImplBase<C, s>::mdcId() const
    {
        dbgAssert(mdcState());
        return mdcState()->_mddb._id;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    const contract::Lid& ImplBase<C, s>::mdcLid() const
    {
        dbgAssert(mdcState());
        return mdcState()->_mddb._lid;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    Side ImplBase<C, s>::mdSide() const
    {
        dbgAssert(mdcState());
        dbgAssert(hmcDescriptor());

        if(hmcDescriptor()->_mdiSideSame)
        {
            return s;
        }

        return invert(s);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    Lid ImplBase<C, s>::mdLid() const
    {
        return Lid {mdcLid(), mdSide()};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    Id ImplBase<C, s>::mdId() const
    {
        return Id {mdcId(), mdSide()};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    bool ImplBase<C, s>::isInvolved() const
    {
        dbgAssert(mdcState());

        if(Side::primary == mdSide())
        {
            return mdcState()->_useCounterOpposite.valStrong() > 0;
        }
        else
        {
            return mdcState()->_useCounterPrimary.valStrong() > 0;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    sbs::Signal<void, bool> ImplBase<C, s>::involvedChanged() const
    {
        dbgAssert(mdcState());

        if(Side::primary == mdSide())
        {
            return mdcState()->_useCounterOpposite.involvedChanged();
        }

        return mdcState()->_useCounterPrimary.involvedChanged();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    auto ImplBase<C, s>::involved() const
    {
        return sbs::signal::Adapter{[](auto&& f, bool v)
        {
            if(v)
            {
                std::forward<decltype(f)>(f)();
            }
        }, involvedChanged()};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    auto ImplBase<C, s>::uninvolved() const
    {
        return sbs::signal::Adapter{[](auto&& f, bool v)
        {
            if(!v)
            {
                std::forward<decltype(f)>(f)();
            }
        }, involvedChanged()};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    const Methods<C, s>* ImplBase<C, s>::methods() const
    {
        return static_cast<const Methods<C, s>*>(static_cast<const MethodsBase<C, s>*>(this));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    const Methods<C, s>* ImplBase<C, s>::operator->() const
    {
        return methods();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    const contract::HmDescriptorBase* ImplBase<C, s>::hmcDescriptor() const
    {
        return this->_stateHolder._hmcDescriptor;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    const contract::HmDescriptorBase*& ImplBase<C, s>::hmcDescriptor()
    {
        return this->_stateHolder._hmcDescriptor;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    contract::MdState* ImplBase<C, s>::mdcState() const
    {
        return this->_stateHolder._mdcState;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    contract::MdState*& ImplBase<C, s>::mdcState()
    {
        return this->_stateHolder._mdcState;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    template <template <Side> class B>
    contract::WiresBundle<B>& ImplBase<C, s>::getWiresBundle() const
    {
        dbgAssertX(mdcState() && hmcDescriptor(), "use uninitialized interface instance");
        return mdcState()->template getWiresBundle<contract::WiresBundle<B>>(hmcDescriptor()->hmbd(contract::HmDescriptor<C, B>::_index)._bundleOffset);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    void ImplBase<C, s>::addRef()
    {
        dbgAssert(!mdcState() == !hmcDescriptor());

        if(mdcState())
        {
            contract::UseCounter& ucOur = (Side::primary == mdSide()) ?
                        mdcState()->_useCounterPrimary :
                        mdcState()->_useCounterOpposite;

            return ucOur.incStrong();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <Side> class C, Side s>
    void ImplBase<C, s>::release()
    {
        dbgAssert(!mdcState() == !hmcDescriptor());

        if(mdcState())
        {
            Generic<false> weakHolder{this->weak()};

            contract::MdState* mdcState = this->mdcState();
            Side mdSide = this->mdSide();

            this->mdcState() = nullptr;
            this->hmcDescriptor() = nullptr;

            contract::UseCounter& ucOur = (Side::primary == mdSide) ?
                        mdcState->_useCounterPrimary :
                        mdcState->_useCounterOpposite;

            contract::UseCounter& ucTheir = (Side::primary == mdSide) ?
                        mdcState->_useCounterOpposite :
                        mdcState->_useCounterPrimary;

            uint32 totalStrong = ucTheir.valStrong() + ucOur.decStrong();
            if(!totalStrong)
            {
                ucTheir.dropSbs();
                ucOur.dropSbs();
                mdcState->_mddb._disconnectWires(mdcState);
            }

            //weakHolder destructs here
        }
    }
}
