// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include "pch.hpp"

namespace dci::module::net
{
    class OptionsStore
    {
        using Options = std::vector<api::Option>;

    public:
        static ExceptionPtr applyOption(poll::descriptor::Native native, const api::Option& op);

        const Options& options() const;
        void pushOptions(const Options& ops);
        void pushOption(const api::Option& op);
        ExceptionPtr applyOptions(poll::descriptor::Native native, bool flush=true);

    private:
        Options _options;
    };
}
