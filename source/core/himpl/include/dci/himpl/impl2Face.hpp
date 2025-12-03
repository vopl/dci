// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "details/implOffsetEvaluator.hpp"
#include <bit>
#include <cstdint>

namespace dci::himpl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl>
    constexpr TFace* impl2Face(TImpl* p) noexcept
    {
        constexpr std::size_t implOffset = details::ImplOffsetEvaluator<TFace>::_value;

        if constexpr(implOffset)
        {
            if(p)
            {
                return std::bit_cast<TFace*>(std::bit_cast<std::uintptr_t>(p)-implOffset);
            }

            return nullptr;
        }

        return std::bit_cast<TFace*>(std::bit_cast<std::uintptr_t>(p)-implOffset);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl>
    constexpr const TFace* impl2Face(const TImpl* cp) noexcept
    {
        constexpr std::size_t implOffset = details::ImplOffsetEvaluator<TFace>::_value;

        if constexpr(implOffset)
        {
            if(cp)
            {
                return std::bit_cast<const TFace*>(std::bit_cast<std::uintptr_t>(cp)-implOffset);
            }

            return nullptr;
        }

        return std::bit_cast<const TFace*>(std::bit_cast<std::uintptr_t>(cp)-implOffset);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl>
    constexpr TFace& impl2Face(TImpl& r) noexcept
    {
        return *impl2Face<TFace, TImpl>(&r);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl>
    constexpr TFace&& impl2Face(TImpl&& rr) noexcept
    {
        return static_cast<TFace&&>(*impl2Face<TFace, TImpl>(&rr));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl>
    constexpr const TFace& impl2Face(const TImpl& cr) noexcept
    {
        return *impl2Face<TFace, TImpl>(&cr);
    }
}
