// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../api.hpp"
#include <dci/utils/integer.hpp>
#include <dci/primitives.hpp>

namespace dci::idl::contract
{
    struct Id
        : Array<uint8, 16>
    {
        static constexpr uint32 _size = 16;

        template <class Int>
        constexpr Array<Int, 16/sizeof(Int)> asArray() const requires(std::is_integral_v<Int>);

        bool API_DCI_IDL fromHex(const String& hex);
        String API_DCI_IDL toHex(uint32 chars = _size*2) const;
    };


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Int>
    inline constexpr Array<Int, 16/sizeof(Int)> Id::asArray() const requires(std::is_integral_v<Int>)
    {
        using Res = Array<Int, _size/sizeof(Int)>;
        Res res {};

        for(std::size_t i{}; i<_size; ++i)
        {
            res[i/sizeof(Int)] |= Int{operator[](i)} << (8 * (i%sizeof(Int)));
        }

        return res;
    }
}
