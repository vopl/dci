// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::transport::natt
{
    class Mapper;
    class Mapping
        : public api::Mapping<>::Opposite
        , public host::module::ServiceBase<Mapping>
    {
    public:
        Mapping();
        ~Mapping();

        void setup(Mapper* mapper);
        const apit::Address& internal() const;

        void setExternal(const apit::Address& external);

    private:
        Mapper *        _mapper{};
        apit::Address   _internal;
        api::Protocol   _protocol{};
        bool            _started{};
        apit::Address   _external;
    };
}
