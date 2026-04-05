/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "sourceAdapter.hpp"

namespace dci::module::www::http::inputSlicer
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SourceAdapter::SourceAdapter(bytes::Alter& data)
        : _data{data}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool SourceAdapter::empty() const
    {
        return _var.visit(utils::overloaded
                          {
                              [this](const Null&){return _data.atEnd();},
                              [](const ForHdr& forHdr){return forHdr.empty();},
                              [](const ForBody& forBody){return forBody.empty();},
                          });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SourceAdapter::Null& SourceAdapter::null()
    {
        return _var.sget<Null>();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SourceAdapter::ForHdr& SourceAdapter::forHdr()
    {
        if(_var.holds<ForHdr>())
            return _var.get<ForHdr>();

        return _var.emplace<ForHdr>(_data);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SourceAdapter::ForBody& SourceAdapter::forBody()
    {
        if(_var.holds<ForBody>())
            return _var.get<ForBody>();

        return _var.emplace<ForBody>(_data);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SourceAdapter::ForHdr::ForHdr(bytes::Alter& data)
        : _data{data}
        , _segment{reinterpret_cast<const char*>(_data.continuousData()), _data.continuousDataSize()}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SourceAdapter::ForHdr::~ForHdr()
    {
        if(_dropped)
            _data.remove(_dropped);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const char* SourceAdapter::ForHdr::segmentBegin() const
    {
        return _segment.data();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const char* SourceAdapter::ForHdr::segmentEnd() const
    {
        return _segment.data() + _segment.size();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t SourceAdapter::ForHdr::segmentSize() const
    {
        return segmentEnd() - segmentBegin();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SourceAdapter::ForHdr::dropFront(std::size_t amount)
    {
        if(_segment.size() > amount)
        {
            _dropped += amount;
            _segment.remove_prefix(amount);
            return;
        }

        _data.remove(_dropped + amount);
        _dropped = 0;

        _segment = std::string_view{reinterpret_cast<const char*>(_data.continuousData()), _data.continuousDataSize()};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool SourceAdapter::ForHdr::empty() const
    {
        return segmentEnd() == segmentBegin();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    char SourceAdapter::ForHdr::front()
    {
        dbgAssert(!empty());
        return *segmentBegin();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SourceAdapter::ForBody::ForBody(bytes::Alter& data)
        : _data{data}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool SourceAdapter::ForBody::empty() const
    {
        return _data.atEnd();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes SourceAdapter::ForBody::detach(uint32 maxSize)
    {
        Bytes res;
        _data.removeTo(res, maxSize);
        return res;
    }

}
