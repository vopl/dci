/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "side.hpp"
#include "id.hpp"
#include "lid.hpp"
#include "../contract/mdDescriptor.hpp"
#include "../contract/mdState.hpp"
#include <compare>

namespace dci::idl::interface
{
    template <template <Side> class C, Side side>
    class ImplBase;

    template <bool strong = false>
    class Generic
    {
    public:
        Generic();
        ~Generic();

    protected:
        explicit Generic(contract::MdState* mdcState, Side mdSide);

    public:
        Generic(const Generic& other);
        Generic(const Generic<!strong>& other);

        explicit Generic(Generic&& other);
        explicit Generic(Generic<!strong>&& other);

        Generic& operator=(const Generic& other);
        Generic& operator=(const Generic<!strong>& other);

        Generic& operator=(Generic&& other);
        Generic& operator=(Generic<!strong>&& other);

    public:
        template <bool oStrong>
        bool operator==(const Generic<oStrong>& other) const;

        template <bool oStrong>
        bool operator!=(const Generic<oStrong>& other) const;

        template <bool oStrong>
        std::strong_ordering operator<=>(const Generic<oStrong>& other) const;

    public:
        template <template <Side> class O, Side oSide>
        Generic(const O<oSide>& other) requires(contract::MdDescriptor<O>::_declared);

        template <template <Side> class O, Side oSide>
        Generic(O<oSide>&& other) requires(contract::MdDescriptor<O>::_declared);

        template <template <Side> class O, Side oSide>
        Generic& operator=(const O<oSide>& other) requires(contract::MdDescriptor<O>::_declared);

        template <template <Side> class O, Side oSide>
        Generic& operator=(O<oSide>&& other) requires(contract::MdDescriptor<O>::_declared);

    public:
        template <template <Side> class O, Side oSide>
        bool operator==(const O<oSide>& other) const requires(contract::MdDescriptor<O>::_declared);

        template <template <Side> class O, Side oSide>
        bool operator!=(const O<oSide>& other) const requires(contract::MdDescriptor<O>::_declared);

        template <template <Side> class O, Side oSide>
        bool operator<=>(const O<oSide>& other) const requires(contract::MdDescriptor<O>::_declared);

    public:
        explicit operator bool() const;
        bool operator!() const;

    public:
        void swap(Generic& other);
        void reset();

    public:
        Generic opposite() const;

        const Generic& weak() const requires(!strong);
        Generic<false> weak() const requires(strong);

        const contract::Id& mdcId() const;
        const contract::Lid& mdcLid() const;
        Side mdSide() const;

        Lid mdLid() const;
        Id mdId() const;

        bool involved() const;

        sbs::Signal<void, bool> involvedChanged() const;

    protected:
        contract::MdState* mdcState() const;

    private:
        void addRef() requires (strong);
        void addRef() requires (!strong);

        void release() requires (strong);
        void release() requires (!strong);

    private:
        template <template <Side> class, Side>
        friend class ImplBase;

        template <bool>
        friend class Generic;

        contract::MdState*  _mdcState;
        Side                _mdSide;
    };



    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    Generic<strong>::Generic()
        : _mdcState{}
        , _mdSide{Side::primary}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    Generic<strong>::~Generic()
    {
        release();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    Generic<strong>::Generic(contract::MdState* mdcState, Side mdSide)
        : _mdcState{mdcState}
        , _mdSide{mdSide}
    {
        addRef();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    Generic<strong>::Generic(const Generic& other)
        : _mdcState{other ? other._mdcState : nullptr}
        , _mdSide{other ? other._mdSide : Side::primary}
    {
        addRef();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    Generic<strong>::Generic(const Generic<!strong>& other)
        : _mdcState{other ? other._mdcState : nullptr}
        , _mdSide{other ? other._mdSide : Side::primary}
    {
        addRef();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    Generic<strong>::Generic(Generic&& other)
        : _mdcState{other ? other._mdcState : nullptr}
        , _mdSide{other ? other._mdSide : Side::primary}
    {
        addRef();
        other.release();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    Generic<strong>::Generic(Generic<!strong>&& other)
        : _mdcState{other ? other._mdcState : nullptr}
        , _mdSide{other ? other._mdSide : Side::primary}
    {
        addRef();
        other.release();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    Generic<strong>& Generic<strong>::operator=(const Generic& other)
    {
        Generic{other}.swap(*this);
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    Generic<strong>& Generic<strong>::operator=(const Generic<!strong>& other)
    {
        Generic{other}.swap(*this);
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    Generic<strong>& Generic<strong>::operator=(Generic&& other)
    {
        Generic{std::move(other)}.swap(*this);
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    Generic<strong>& Generic<strong>::operator=(Generic<!strong>&& other)
    {
        Generic{std::move(other)}.swap(*this);
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    template <bool oStrong>
    bool Generic<strong>::operator==(const Generic<oStrong>& other) const
    {
        return _mdcState == other._mdcState;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    template <bool oStrong>
    bool Generic<strong>::operator!=(const Generic<oStrong>& other) const
    {
        return !operator==(other);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    template <bool oStrong>
    std::strong_ordering Generic<strong>::operator<=>(const Generic<oStrong>& other) const
    {
        return _mdcState <=> other._mdcState;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    template <template <Side> class O, Side oSide>
    Generic<strong>::Generic(const O<oSide>& other) requires(contract::MdDescriptor<O>::_declared)
    {
        if(other)
        {
            _mdcState = other.mdcState();
            _mdSide = other.mdSide();
            addRef();
        }
        else
        {
            _mdcState = nullptr;
            _mdSide = Side::primary;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    template <template <Side> class O, Side oSide>
    Generic<strong>::Generic(O<oSide>&& other) requires(contract::MdDescriptor<O>::_declared)
    {
        if(other)
        {
            _mdcState = other.mdcState();
            _mdSide = other.mdSide();
            addRef();
            other.release();
        }
        else
        {
            _mdcState = nullptr;
            _mdSide = Side::primary;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    template <template <Side> class O, Side oSide>
    Generic<strong>& Generic<strong>::operator=(const O<oSide>& other) requires(contract::MdDescriptor<O>::_declared)
    {
        return (*this) = Generic{other};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    template <template <Side> class O, Side oSide>
    Generic<strong>& Generic<strong>::operator=(O<oSide>&& other) requires(contract::MdDescriptor<O>::_declared)
    {
        return (*this) = Generic{std::move(other)};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    template <template <Side> class O, Side oSide>
    bool Generic<strong>::operator==(const O<oSide>& other) const requires(contract::MdDescriptor<O>::_declared)
    {
        return _mdcState == other.mdcState();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    template <template <Side> class O, Side oSide>
    bool Generic<strong>::operator!=(const O<oSide>& other) const requires(contract::MdDescriptor<O>::_declared)
    {
        return !operator==(other);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    template <template <Side> class O, Side oSide>
    bool Generic<strong>::operator<=>(const O<oSide>& other) const requires(contract::MdDescriptor<O>::_declared)
    {
        return _mdcState <=> other.mdcState();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    Generic<strong>::operator bool() const
    {
        if constexpr(strong)
        {
            return !!_mdcState;
        }
        else
        {
            return _mdcState &&
                    (_mdcState->_useCounterOpposite.valStrong() || _mdcState->_useCounterPrimary.valStrong());
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    bool Generic<strong>::operator!() const
    {
        return !operator bool();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    void Generic<strong>::swap(Generic& other)
    {
        std::swap(_mdcState, other._mdcState);
        std::swap(_mdSide, other._mdSide);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    void Generic<strong>::reset()
    {
        release();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    Generic<strong> Generic<strong>::opposite() const
    {
        if constexpr(strong)
        {
            return Generic{_mdcState, invert(_mdSide)};
        }
        else
        {
            if(operator bool())
            {
                return Generic{_mdcState, invert(_mdSide)};
            }
            else
            {
                return Generic{};
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    const Generic<strong>& Generic<strong>::weak() const requires(!strong)
    {
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    Generic<false> Generic<strong>::weak() const requires(strong)
    {
        return Generic<false>{*this};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    const contract::Id& Generic<strong>::mdcId() const
    {
        dbgAssert(_mdcState);
        return _mdcState->_mddb._id;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    const contract::Lid& Generic<strong>::mdcLid() const
    {
        dbgAssert(_mdcState);
        return _mdcState->_mddb._lid;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    Side Generic<strong>::mdSide() const
    {
        return _mdSide;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    Lid Generic<strong>::mdLid() const
    {
        return Lid {mdcLid(), mdSide()};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    Id Generic<strong>::mdId() const
    {
        return Id {mdcId(), mdSide()};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    bool Generic<strong>::involved() const
    {
        dbgAssert(_mdcState);

        if(Side::primary == mdSide())
        {
            return _mdcState->_useCounterOpposite.valStrong() > 0;
        }
        else
        {
            return _mdcState->_useCounterPrimary.valStrong() > 0;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    sbs::Signal<void, bool> Generic<strong>::involvedChanged() const
    {
        dbgAssert(operator bool());

        if(Side::primary == mdSide())
        {
            return _mdcState->_useCounterOpposite.involvedChanged();
        }

        return _mdcState->_useCounterPrimary.involvedChanged();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    contract::MdState* Generic<strong>::mdcState() const
    {
        return _mdcState;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    void Generic<strong>::addRef() requires (strong)
    {
        if(_mdcState)
        {
            contract::UseCounter& ucOur = (Side::primary == mdSide()) ?
                        _mdcState->_useCounterPrimary :
                        _mdcState->_useCounterOpposite;

            return ucOur.incStrong();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    void Generic<strong>::addRef() requires (!strong)
    {
        if(_mdcState)
        {
            contract::UseCounter& ucOur = (Side::primary == mdSide()) ?
                        _mdcState->_useCounterPrimary :
                        _mdcState->_useCounterOpposite;

            return ucOur.incWeak();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    void Generic<strong>::release() requires (strong)
    {
        if(_mdcState)
        {
            Generic<false> weakHolder{this->weak()};

            contract::MdState* mdcState = _mdcState;
            Side mdSide = _mdSide;

            _mdcState = nullptr;
            _mdSide = Side::primary;

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

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <bool strong>
    void Generic<strong>::release() requires (!strong)
    {
        if(_mdcState)
        {
            contract::MdState* mdcState = _mdcState;
            Side mdSide = _mdSide;

            _mdcState = nullptr;
            _mdSide = Side::primary;

            contract::UseCounter& ucOur = (Side::primary == mdSide) ?
                        mdcState->_useCounterPrimary :
                        mdcState->_useCounterOpposite;

            contract::UseCounter& ucTheir = (Side::primary == mdSide) ?
                        mdcState->_useCounterOpposite :
                        mdcState->_useCounterPrimary;

            uint32 totalStrong = ucTheir.valStrong() + ucOur.valStrong();
            if(!totalStrong)
            {
                uint32 totalWeak = ucTheir.valWeak() + ucOur.decWeak();
                if(!totalWeak)
                {
                    mdcState->_mddb._destroyMds(mdcState);
                }
            }
            else
            {
                ucOur.decWeak();
            }
        }
    }
}
