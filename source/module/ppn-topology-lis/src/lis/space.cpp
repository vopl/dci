/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "space.hpp"

namespace dci::module::ppn::topology::lis::space
{
    static_assert(Id{}.size() >= sizeof(Sid));
    static_assert(sizeof(Sid) == sizeof(Csid));

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sid& sid_l(Id& id)
    {
        return *static_cast<Sid*>(static_cast<void *>(id.data() + id.size() - sizeof(Sid)));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Sid& sid_l(const Id& id)
    {
        return *static_cast<const Sid*>(static_cast<const void *>(id.data() + id.size() - sizeof(Sid)));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sid sid(const Id& id)
    {
        return utils::endian::l2n(sid_l(id));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void setSid(Id& id, Sid sid)
    {
        sid_l(id) = utils::endian::n2l(sid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Id id(Sid sid)
    {
        Id res {};
        setSid(res, sid);
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Csid csid(Sid sidBase, Sid sidTarget)
    {
        SmallNum nBase = static_cast<SmallNum>(sidBase);
        SmallNum nTarget = static_cast<SmallNum>(sidTarget);

        SmallNum diff = nTarget - nBase;

        static constexpr SmallNum max = (~SmallNum{})/2;

        if(diff <= max)
        {
            diff *= 2;
        }
        else
        {
            diff = SmallNum{} - diff;
            diff *= 2;
            diff += 1;
        }

        return static_cast<Csid>(diff);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Csid csid(const Id& idBase, const Id& idTarget)
    {
        return csid(sid(idBase), sid(idTarget));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::vector<Range> ranges(Sid sidBase, Csid csidTargetMin, Csid csidTargetMax)
    {
        SmallNum nBase = static_cast<SmallNum>(sidBase);
        SmallNum nTargetMin = static_cast<SmallNum>(csidTargetMin)/2;
        SmallNum nTargetMax = static_cast<SmallNum>(csidTargetMax)/2;

        dbgAssert(nTargetMin <= nTargetMax);

        std::vector<Range> res;

        auto pushRanges = [&](SmallNum min, SmallNum max)
        {
            if(min == max) return;

            if(min < max)
            {
                res.push_back(Range{min, max});
                return;
            }

            res.push_back(Range{min, ~SmallNum{}});
            res.push_back(Range{SmallNum{}, max});
            return;
        };

        pushRanges(nBase - nTargetMax-1, nBase - nTargetMin+1);
        pushRanges(nBase + nTargetMin-1, nBase + nTargetMax+1);

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::vector<Range> ranges(const Id& idBase, Csid csidTargetMin, Csid csidTargetMax)
    {
        return ranges(sid(idBase), csidTargetMin, csidTargetMax);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SmallNum dist(const Id& id1, const Id& id2)
    {
        SmallNum diff = static_cast<SmallNum>(sid(id1)) - static_cast<SmallNum>(sid(id2));

        static constexpr SmallNum max = (~SmallNum{})/2;

        if(diff <= max)
        {
            return diff;
        }

        return SmallNum{} - diff;
    }
}
