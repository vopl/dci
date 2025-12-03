// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "transformHolder/call.hpp"
#include "transformHolder/voidStream.hpp"
#include "transformHolder/autoTag.hpp"

#include <type_traits>

namespace dci::cmt::details
{

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<
        class F,
        class TSourceStream = void,
        template<class...> class TDestinationStream = transformHolder::VoidStream,
        class TDestination = transformHolder::AutoTag>
    class TransformHolder
    {

    private:
        using Call = transformHolder::Call<F, TSourceStream, TDestinationStream, TDestination>;

    public:
        static constexpr bool _valid = Call::_valid;

        using SourceStream = TSourceStream;
        using DestinationStream = typename Call::DestinationStream;

        using TransformResult = typename Call::DestinationStreamOpposite;

    public:

        TransformHolder(F&& f)
            : _call(std::forward<F>(f))
        {
        }

        auto invoke() requires(_valid && std::is_void_v<SourceStream> && std::is_void_v<DestinationStream>)
        {
            return _call.invoke(nullptr, nullptr);
        }

        auto invoke(SourceStream* source) requires(_valid && !std::is_void_v<SourceStream> && std::is_void_v<DestinationStream>)
        {
            return _call.invoke(source, nullptr);
        }

        auto invoke(DestinationStream* destination) requires(_valid && std::is_void_v<SourceStream> && !std::is_void_v<DestinationStream>)
        {
            return _call.invoke(nullptr, destination);
        }

        auto invoke(SourceStream* source, DestinationStream* destination) requires(_valid && !std::is_void_v<SourceStream> && !std::is_void_v<DestinationStream>)
        {
            return _call.invoke(source, destination);
        }

    private:
        Call _call;
    };
}
