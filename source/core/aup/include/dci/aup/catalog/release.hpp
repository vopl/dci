// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "object.hpp"

namespace dci::aup::catalog
{
    struct Release : Object
    {
        String _srcBranch;
        String _srcRevision;
        uint64 _srcMoment {};//unix time

        String _platformOs;
        String _platformArch;
        String _compiler;
        String _compilerVersion;
        String _compilerOptimization;

        uint32 _stability {};

        String _vendor;
        Array<uint8, 32> _vendorSign {};
        Array<uint8, 64> _signature {};

        Type type() const override {return Object::Type::release;}
    };

    using ReleasePtr = std::unique_ptr<Release>;
}
