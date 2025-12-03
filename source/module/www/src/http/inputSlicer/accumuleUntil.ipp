// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "accumuleUntil.hpp"

namespace dci::module::www::http::inputSlicer
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <char terminator, Result tooBig, class Accumuler>
    Result accumuleUntil(SourceAdapter& source, Accumuler& accumuler)
    {
        SourceAdapter::ForHdr& sourceForHdr = source.forHdr();
        while(!sourceForHdr.empty())
        {
            std::size_t availSize = std::min(sourceForHdr.segmentSize(), Accumuler::_limit - accumuler.size() + 1);
            auto availBegin = sourceForHdr.segmentBegin();
            auto availEnd = availBegin+availSize;
            auto foundIter = std::find(availBegin, availEnd, terminator);

            std::size_t size4Accumule = foundIter - availBegin;

            if(Accumuler::_limit < accumuler.size() + size4Accumule)
                return tooBig;

            accumuler.append(availBegin, foundIter);

            if(availEnd == foundIter)
                sourceForHdr.dropFront(size4Accumule);
            else
            {
                sourceForHdr.dropFront(size4Accumule+1);
                return Result::done;
            }
        }

        return Result::needMore;
    }
}
