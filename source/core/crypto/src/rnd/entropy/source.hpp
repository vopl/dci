// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <memory>

namespace dci::crypto::rnd
{
    class Instance;
}

namespace dci::crypto::rnd::entropy
{
    class Source;
    using SourcePtr = std::unique_ptr<Source>;

    class Source
    {
    public:
        Source(Instance* instance);
        virtual ~Source();

        virtual void flush() = 0;

    protected:
        Instance* _instance;
    };
}
