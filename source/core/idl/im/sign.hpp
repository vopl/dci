// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <string>
#include <cstring>
#include <cstdint>

namespace dci::idl::im
{
    class Sign final
    {
    public:
        static constexpr std::size_t _size = 16;

    public:
        Sign();
        Sign(const std::uint8_t (&data)[_size]);
        Sign(const Sign& from);
        Sign(Sign&& from);
        ~Sign();

        Sign& operator=(const Sign& from);

        template <std::size_t N>
        Sign& operator=(const std::array<std::uint8_t, N>& from) requires (N >= _size);

        std::uint8_t* data();
        const std::uint8_t* data() const;

        std::string toHex(std::size_t charStart=0, std::size_t chars=_size*2) const;
        bool fromHex(const std::string& txt);
        void fromRnd();

        bool operator<(const Sign& with) const;
        bool operator>(const Sign& with) const;
        bool operator<=(const Sign& with) const;
        bool operator>=(const Sign& with) const;
        bool operator==(const Sign& with) const;
        bool operator!=(const Sign& with) const;

    private:
        std::uint8_t _data[_size];
    };

    Sign operator^(const Sign& a, const Sign& b);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t N>
    Sign& Sign::operator=(const std::array<std::uint8_t, N>& from) requires (N >= _size)
    {
        std::memcpy(_data, from.data(), _size);
        return *this;
    }
}
