// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "details/implOffsetEvaluator.hpp"
#include <bit>

namespace dci::himpl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace>
    constexpr typename TFace::Impl* face2Impl(TFace* p) noexcept
    {
        constexpr std::size_t implOffset = details::ImplOffsetEvaluator<TFace>::_value;

        if constexpr(implOffset)
        {
            if(p)
            {
                return std::bit_cast<typename TFace::Impl *>(std::bit_cast<std::uintptr_t>(p)+implOffset);
            }

            return nullptr;
        }

        return std::bit_cast<typename TFace::Impl *>(std::bit_cast<std::uintptr_t>(p)+implOffset);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace>
    constexpr const typename TFace::Impl* face2Impl(const TFace* cp) noexcept
    {
        constexpr std::size_t implOffset = details::ImplOffsetEvaluator<TFace>::_value;

        if constexpr(implOffset)
        {
            if(cp)
            {
                return std::bit_cast<const typename TFace::Impl *>(std::bit_cast<std::uintptr_t>(cp)+implOffset);
            }

            return nullptr;
        }

        return std::bit_cast<const typename TFace::Impl *>(std::bit_cast<std::uintptr_t>(cp)+implOffset);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace>
    constexpr typename TFace::Impl& face2Impl(TFace& r) noexcept
    {
        return *face2Impl<TFace>(&r);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace>
    constexpr typename TFace::Impl&& face2Impl(TFace&& rr) noexcept
    {
        return static_cast<typename TFace::Impl&&>(*face2Impl<TFace>(&rr));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace>
    constexpr const typename TFace::Impl& face2Impl(const TFace& cr) noexcept
    {
        return *face2Impl<TFace>(&cr);
    }
}
