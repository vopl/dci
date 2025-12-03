// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::bytes::performing
{
    auto execute(auto&& promotor, auto&& stepPerformer) -> auto
    {
        for(;;)
        {
            uint32 size = promotor.possibleContinuousSize();

            promotor.promotePrepare(size);

            auto res = stepPerformer(promotor.continuousData(), size);

            promotor.promoteFix(size);

            if(res._finish)
            {
                return res._value;
            }
        }
    }

    auto execute(auto&& lhsPromotor, auto&& rhsPromotor, auto&& stepPerformer) -> auto
    {
        for(;;)
        {
            uint32 size = std::min(lhsPromotor.possibleContinuousSize(), rhsPromotor.possibleContinuousSize());

            lhsPromotor.promotePrepare(size);
            rhsPromotor.promotePrepare(size);

            auto res = stepPerformer(lhsPromotor.continuousData(), rhsPromotor.continuousData(), size);

            lhsPromotor.promoteFix(size);
            rhsPromotor.promoteFix(size);

            if(res._finish)
            {
                return res._value;
            }
        }
    }

}
