// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <type_traits>
#include <cstddef>
#include <cstdint>

namespace dci::himpl::details
{
    template <class TFace, bool=std::is_polymorphic<TFace>::value, bool=std::is_polymorphic<typename TFace::Layout>::value>
    struct ImplOffsetEvaluator
    {
        static constexpr std::size_t _value = 0;
    };

    template <class TFace>
    struct ImplOffsetEvaluator<TFace, true, false>
    {
        struct VTableProbe
        {
            void* _vTablePtr;
            alignas(TFace::Layout) std::byte x[sizeof(TFace::Layout)];
        };

        static constexpr std::size_t _value = offsetof(VTableProbe, x);
    };

    template <class TFace>
    struct ImplOffsetEvaluator<TFace, true, true>
    {
        static constexpr std::size_t _value = TFace::Layout::_implOffset;
    };
}
