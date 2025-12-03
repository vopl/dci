// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "output.hpp"
#include "../localEdge.hpp"

namespace dci::module::stiac::localEdge
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Output::Output(Bytes &data)
        : _data{ data }
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Output::~Output()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    link::Sink Output::makeSink(uint32 reserveIfCan)
    {
        dbgAssert(!_hasActiveSink);
        _hasActiveSink = true;

        dbgAssert(!_reserved);

        bytes::Alter a(_data.end());
        if(reserveIfCan && !a.sizeBack())
        {
            _reserved = reserveIfCan;
            a.advance(static_cast<int32>(reserveIfCan));
        }

        return link::Sink(this, std::move(a));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Output::finalize(link::Sink& sink, bytes::Alter&& buffer)
    {
        dbgAssert(_hasActiveSink);
        _hasActiveSink = false;

        (void)sink;

        if(_reserved)
        {
            dbgAssert(_reserved <= buffer.sizeBack());
            bytes::Alter{std::move(buffer)};
            _data.begin().remove(_reserved);
            _reserved = 0;
        }
        else
        {
            bytes::Alter{std::move(buffer)};
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::pair<uint32, bool> Output::mapTuid(const std::array<uint8, 16>& tuid)
    {
        auto res = _tuidMap.try_emplace(tuid, uint32(_tuidMap.size()));
        return std::make_pair(res.first->second, res.second);
    }
}
