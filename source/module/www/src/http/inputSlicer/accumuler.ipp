// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "accumuler.hpp"

namespace dci::module::www::http::inputSlicer
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t limit>
    void Accumuler<std::string, limit>::reset()
    {
        if(_downstream.capacity() > 64)
            _downstream = {};
        else
            _downstream.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t limit>
    template <class Iter>
    void Accumuler<std::string, limit>::append(Iter begin, Iter end)
    {
        dbgAssert(_downstream.size() + (end-begin) <= _limit);
        _downstream.append(begin, end);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t limit>
    std::size_t Accumuler<std::string, limit>::size() const
    {
        return _downstream.size();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t limit>
    bool Accumuler<std::string, limit>::empty() const
    {
        return _downstream.empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t limit>
    std::string_view Accumuler<std::string, limit>::str() const
    {
        return _downstream;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t limit>
    std::ostream& operator<<(std::ostream& ostr, const Accumuler<std::string, limit>& acc)
    {
        return ostr << acc._downstream;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t limit>
    void Accumuler<std::array<char, limit>, limit>::reset()
    {
        _size = 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t limit>
    template <class Iter>
    void Accumuler<std::array<char, limit>, limit>::append(Iter begin, Iter end)
    {
        dbgAssert(_size + (end-begin) <= _limit);
        std::copy(begin, end, _downstream.begin() + _size);
        _size += (end-begin);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t limit>
    std::size_t Accumuler<std::array<char, limit>, limit>::size() const
    {
        return _size;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t limit>
    bool Accumuler<std::array<char, limit>, limit>::empty() const
    {
        return !_size;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t limit>
    std::string_view Accumuler<std::array<char, limit>, limit>::str() const
    {
        return {_downstream.data(), _size};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t limit>
    std::ostream& operator<<(std::ostream& ostr, const Accumuler<std::array<char, limit>, limit>& acc)
    {
        return ostr << std::string{acc._downstream.data(), acc._size};
    }
}
