// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "performer.hpp"

namespace dci::module::ppn::transport::natt::mapper
{
    struct PerformerBlank
    {
        using Builder = std::function<PerformerPtr()>;
        Builder _builder;
        int     _priority {};
    };

    class Service
    {
    public:
        Service();
        virtual ~Service();

        virtual const std::string& name() const;
        virtual PerformerBlank candidate(const apit::Address& internal) = 0;

    protected:
        std::string _name {"unnamed"};
    };
}
