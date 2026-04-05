/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "details/polymorphicFaceLayout.hpp"
#include "details/simpleFaceLayout.hpp"
#include "details/implDestructionExecutor.hpp"
#include "check/isFirstPolymorphic.hpp"
#include "check/runtimeInstance.hpp"
#include "check/faceConstraints.hpp"
#include "fakeConstructionArg.hpp"

#include <tuple>

namespace dci::himpl
{
    template <class TFace, class TImpl, class... TBases>
    requires check::FaceLayoutParams<TFace, TImpl, TBases...>
    class FaceLayout
        : public
            std::conditional_t<
                check::IsFirstPolymorphic<TBases...>::_value,
                details::PolymorphicFaceLayout<TImpl, TBases...>,
                details::SimpleFaceLayout<TImpl, TBases...>
            >
    {

    public:
        using Layout =
                std::conditional_t<
                    check::IsFirstPolymorphic<TBases...>::_value,
                    details::PolymorphicFaceLayout<TImpl, TBases...>,
                    details::SimpleFaceLayout<TImpl, TBases...>
                >;
    public:
        using Face = TFace;
        using Impl = TImpl;
        using BasesTuple = std::tuple<TBases...>;

    protected:
        FaceLayout(FakeConstructionArg fc);

        template <class... Args>
        FaceLayout(Args&&... args);

        FaceLayout()
            noexcept(ImplMetaInfo<TImpl>::_hasNothrowDefaultCtor)
            requires(ImplMetaInfo<TImpl>::_hasDefaultCtor);

        ~FaceLayout()
            noexcept(ImplMetaInfo<TImpl>::_hasNothrowDestructor)
            requires(ImplMetaInfo<TImpl>::_hasDestructor);

    protected:
        FaceLayout(const TFace& other)
            noexcept(ImplMetaInfo<TImpl>::_hasNothrowCopyCtor)
            requires(ImplMetaInfo<TImpl>::_hasCopyCtor);

        FaceLayout(TFace&& other)
            noexcept(ImplMetaInfo<TImpl>::_hasNothrowMoveCtor)
            requires(ImplMetaInfo<TImpl>::_hasMoveCtor);

        FaceLayout& operator=(const TFace& other)
            noexcept(ImplMetaInfo<TImpl>::_hasNothrowCopyAssign)
            requires(ImplMetaInfo<TImpl>::_hasCopyAssign);

        FaceLayout& operator=(TFace&& other)
            noexcept(ImplMetaInfo<TImpl>::_hasNothrowMoveAssign)
            requires(ImplMetaInfo<TImpl>::_hasMoveAssign);

    protected:
        FaceLayout(const TImpl& other)
            noexcept(ImplMetaInfo<TImpl>::_hasNothrowCopyCtor)
            requires(ImplMetaInfo<TImpl>::_hasCopyCtor);

        FaceLayout(TImpl&& other)
            noexcept(ImplMetaInfo<TImpl>::_hasNothrowMoveCtor)
            requires(ImplMetaInfo<TImpl>::_hasMoveCtor);

        FaceLayout& operator=(const TImpl& other)
            noexcept(ImplMetaInfo<TImpl>::_hasNothrowCopyAssign)
            requires(ImplMetaInfo<TImpl>::_hasCopyAssign);

        FaceLayout& operator=(TImpl&& other)
            noexcept(ImplMetaInfo<TImpl>::_hasNothrowMoveAssign)
            requires(ImplMetaInfo<TImpl>::_hasMoveAssign);

    protected:
        TImpl* pimpl() noexcept;
        TImpl& impl() noexcept;

        const TImpl* pimpl() const noexcept;
        const TImpl& impl() const noexcept;
    };













    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl, class... TBases>
    requires check::FaceLayoutParams<TFace, TImpl, TBases...>
    FaceLayout<TFace, TImpl, TBases...>::FaceLayout(FakeConstructionArg fc)
        : Layout{fc}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl, class... TBases>
    requires check::FaceLayoutParams<TFace, TImpl, TBases...>
    template <class... Args>
    FaceLayout<TFace, TImpl, TBases...>::FaceLayout(Args&&... args)
        : Layout{FakeConstructionArg()}
    {
        new (pimpl()) TImpl{std::forward<Args>(args)...};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl, class... TBases>
    requires check::FaceLayoutParams<TFace, TImpl, TBases...>
    FaceLayout<TFace, TImpl, TBases...>::FaceLayout()
        noexcept(ImplMetaInfo<TImpl>::_hasNothrowDefaultCtor)
        requires(ImplMetaInfo<TImpl>::_hasDefaultCtor)
        : Layout{FakeConstructionArg()}
    {
        new (pimpl()) TImpl{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl, class... TBases>
    requires check::FaceLayoutParams<TFace, TImpl, TBases...>
    FaceLayout<TFace, TImpl, TBases...>::~FaceLayout()
        noexcept(ImplMetaInfo<TImpl>::_hasNothrowDestructor)
        requires(ImplMetaInfo<TImpl>::_hasDestructor)
    {
#ifndef NDEBUG
        volatile auto checker = check::RuntimeInstance<TFace>::_initiator;
        (void)checker;
#endif

        details::ImplDestructionExecutor<
            TImpl,
            (std::is_final<TFace>::value && 0 == sizeof...(TBases)) ||
            (ImplMetaInfo<TImpl>::_isFinal && 0 == std::tuple_size<typename ImplMetaInfo<TImpl>::BasesTuple>::value)
        >::exec(pimpl());
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl, class... TBases>
    requires check::FaceLayoutParams<TFace, TImpl, TBases...>
    FaceLayout<TFace, TImpl, TBases...>::FaceLayout(const TFace& other)
        noexcept(ImplMetaInfo<TImpl>::_hasNothrowCopyCtor)
        requires(ImplMetaInfo<TImpl>::_hasCopyCtor)
        : Layout{FakeConstructionArg()}
    {
        new (pimpl()) TImpl{other.impl()};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl, class... TBases>
    requires check::FaceLayoutParams<TFace, TImpl, TBases...>
    FaceLayout<TFace, TImpl, TBases...>::FaceLayout(TFace&& other)
        noexcept(ImplMetaInfo<TImpl>::_hasNothrowMoveCtor)
        requires(ImplMetaInfo<TImpl>::_hasMoveCtor)
        : Layout{FakeConstructionArg()}
    {
        new (pimpl()) TImpl{std::move(other.impl())};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl, class... TBases>
    requires check::FaceLayoutParams<TFace, TImpl, TBases...>
    FaceLayout<TFace, TImpl, TBases...>& FaceLayout<TFace, TImpl, TBases...>::operator=(const TFace& other)
        noexcept(ImplMetaInfo<TImpl>::_hasNothrowCopyAssign)
        requires(ImplMetaInfo<TImpl>::_hasCopyAssign)
    {
        impl() = other.impl();
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl, class... TBases>
    requires check::FaceLayoutParams<TFace, TImpl, TBases...>
    FaceLayout<TFace, TImpl, TBases...>& FaceLayout<TFace, TImpl, TBases...>::operator=(TFace&& other)
        noexcept(ImplMetaInfo<TImpl>::_hasNothrowMoveAssign)
        requires(ImplMetaInfo<TImpl>::_hasMoveAssign)
    {
        impl() = std::move(other.impl());
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl, class... TBases>
    requires check::FaceLayoutParams<TFace, TImpl, TBases...>
    FaceLayout<TFace, TImpl, TBases...>::FaceLayout(const TImpl& other)
        noexcept(ImplMetaInfo<TImpl>::_hasNothrowCopyCtor)
        requires(ImplMetaInfo<TImpl>::_hasCopyCtor)
        : Layout{FakeConstructionArg()}
    {
        new (pimpl()) TImpl{other};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl, class... TBases>
    requires check::FaceLayoutParams<TFace, TImpl, TBases...>
    FaceLayout<TFace, TImpl, TBases...>::FaceLayout(TImpl&& other)
        noexcept(ImplMetaInfo<TImpl>::_hasNothrowMoveCtor)
        requires(ImplMetaInfo<TImpl>::_hasMoveCtor)
        : Layout{FakeConstructionArg()}
    {
        new (pimpl()) TImpl{std::move(other)};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl, class... TBases>
    requires check::FaceLayoutParams<TFace, TImpl, TBases...>
    FaceLayout<TFace, TImpl, TBases...>& FaceLayout<TFace, TImpl, TBases...>::operator=(const TImpl& other)
        noexcept(ImplMetaInfo<TImpl>::_hasNothrowCopyAssign)
        requires(ImplMetaInfo<TImpl>::_hasCopyAssign)
    {
        impl() = other;
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl, class... TBases>
    requires check::FaceLayoutParams<TFace, TImpl, TBases...>
    FaceLayout<TFace, TImpl, TBases...>& FaceLayout<TFace, TImpl, TBases...>::operator=(TImpl&& other)
        noexcept(ImplMetaInfo<TImpl>::_hasNothrowMoveAssign)
        requires(ImplMetaInfo<TImpl>::_hasMoveAssign)
    {
        impl() = std::move(other);
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl, class... TBases>
    requires check::FaceLayoutParams<TFace, TImpl, TBases...>
    TImpl* FaceLayout<TFace, TImpl, TBases...>::pimpl() noexcept
    {
        return face2Impl(static_cast<TFace*>(this));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl, class... TBases>
    requires check::FaceLayoutParams<TFace, TImpl, TBases...>
    TImpl& FaceLayout<TFace, TImpl, TBases...>::impl() noexcept
    {
        return *pimpl();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl, class... TBases>
    requires check::FaceLayoutParams<TFace, TImpl, TBases...>
    const TImpl* FaceLayout<TFace, TImpl, TBases...>::pimpl() const noexcept
    {
        return face2Impl(static_cast<const TFace*>(this));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl, class... TBases>
    requires check::FaceLayoutParams<TFace, TImpl, TBases...>
    const TImpl& FaceLayout<TFace, TImpl, TBases...>::impl() const noexcept
    {
        return *pimpl();
    }
}
