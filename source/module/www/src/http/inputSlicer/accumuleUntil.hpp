// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "result.hpp"
#include "sourceAdapter.hpp"

namespace dci::module::www::http::inputSlicer
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <char terminator, Result tooBig, class Accumuler>
    Result accumuleUntil(SourceAdapter& source, Accumuler& accumuler);
}

#include "accumuleUntil.ipp"
