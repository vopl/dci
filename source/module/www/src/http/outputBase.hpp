// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "io/outputBase.hpp"
#include "compress/unified.hpp"

namespace dci::module::www::http
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api>
    class OutputBase
        : public io::OutputBase<Support, Impl, Api>
    {
        using Base = io::OutputBase<Support, Impl, Api>;

    public:
        OutputBase(Support* support, Api&& api);
        ~OutputBase();

    private:
        using Compressor = compress::Unified<compress::Direction::compress>;
        Compressor  _compressor;
        bool        _chunked{};
    };
}

#include "outputBase.ipp"
