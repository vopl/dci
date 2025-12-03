// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "natt/mapper.hpp"

namespace dci::module::ppn::transport
{
    class Natt
        : public apit::Natt<>::Opposite
        , public host::module::ServiceBase<Natt>
    {
    public:
        Natt(dci::host::module::Entry* module);
        ~Natt();

    private:
        natt::Mapper _mapper;
    };
}
