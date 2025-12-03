// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::www::http::inputSlicer
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Downstream, std::size_t limit = Downstream{}.size()> struct Accumuler;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t limit> struct Accumuler<std::string, limit>
    {
        static constexpr std::size_t _limit = limit;
        std::string _downstream;

        void reset();
        template <class Iter> void append(Iter begin, Iter end);
        std::size_t size() const;
        bool empty() const;
        std::string_view str() const;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t limit>
    std::ostream& operator<<(std::ostream& ostr, const Accumuler<std::string, limit>& acc);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t limit> struct Accumuler<std::array<char, limit>, limit>
    {
        static constexpr std::size_t _limit = limit;
        alignas(8) std::array<char, _limit>  _downstream;
        std::size_t _size{};

        void reset();
        template <class Iter> void append(Iter begin, Iter end);
        std::size_t size() const;
        bool empty() const;
        std::string_view str() const;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t limit>
    std::ostream& operator<<(std::ostream& ostr, const Accumuler<std::array<char, limit>, limit>& acc);
}

#include "accumuler.ipp"
