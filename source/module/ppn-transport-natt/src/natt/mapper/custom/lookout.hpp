// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../lookout.hpp"
#include "../service.hpp"
#include "../performer.hpp"

namespace dci::module::ppn::transport::natt::mapper::custom
{
    class Lookout
        : public mapper::Lookout
        , private mapper::Service
    {
    public:
        using Maps = std::multimap<apit::Address, apit::Address>;

    public:
        Lookout(Mapper* mapper);
        ~Lookout() override;

        void map(const apit::Address& i, const apit::Address& e);
        void unmap(const apit::Address& i, const apit::Address& e);

        PerformerBlank candidate(const apit::Address& internal) override;

        const Maps& maps() const;
        std::size_t revision() const;
        cmt::Waitable& changed();

    private:
        Maps        _maps;
        std::size_t _revision{};
        cmt::Pulser _changed;
    };
}
