// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::node
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Logger
        : public api::Logger<>::Opposite
        , public host::module::ServiceBase<Logger>
    {
    public:
        Logger();
        ~Logger();

        bool test(const auto& key);
        bool test(const auto& prefix, const auto& head, const auto&... tail);

    private:
        std::map<uint64, bool> _config;
    };
}
