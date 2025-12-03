// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "service.hpp"

namespace dci::module::ppn::transport::natt
{
    class Mapper;
}

namespace dci::module::ppn::transport::natt::mapper
{
    class Lookout
    {
    public:
        Lookout(Mapper* mapper);
        virtual ~Lookout();

        PerformerBlank candidate(const apit::Address& internal);

    public:
        bool activate(Service* s);
        bool deactivate(Service* s);

        Mapper* mapper();

    private:
        Mapper* _mapper;
        std::set<Service*> _services;
    };
}
