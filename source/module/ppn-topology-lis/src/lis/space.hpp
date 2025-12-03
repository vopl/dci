// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::topology::lis::space
{
    //regular ppn identifier
    using Id = node::link::Id;

    using SmallNum = uint64;

    //small identifier - Id tip
    enum Sid : SmallNum {};

    //local centered small identifier - Sid normalized by local node id
    enum Csid : SmallNum {};

    //tip little endian
    Sid& sid_l(Id& id);
    const Sid& sid_l(const Id& id);

    //tip native endian
    Sid sid(const Id& id);

    void setSid(Id& id, Sid sid);
    Id id(Sid sid);

    Csid csid(Sid sidBase, Sid sidTarget);
    Csid csid(const Id& idBase, const Id& idTarget);

    struct Range
    {
        SmallNum _min {};
        SmallNum _max {};
    };

    std::vector<Range> ranges(Sid sidBase, Csid csidTargetMin, Csid csidTargetMax);
    std::vector<Range> ranges(const Id& idBase, Csid csidTargetMin, Csid csidTargetMax);

    SmallNum dist(const Id& id1, const Id& id2);
}
