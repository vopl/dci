// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "input.hpp"

namespace dci::module::stiac::localEdge
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Input::Input()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Input::~Input()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Input::append(Bytes&& data)
    {
        _data.end().write(std::move(data));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Input::empty() const
    {
        return _data.empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    link::Source Input::makeSource()
    {
        dbgAssert(!_hasActiveSource);
        dbgAssert(!empty());

        _hasActiveSource = true;
        return link::Source(this, _data.begin());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Input::finalize(link::Source& source, bytes::Alter&& buffer)
    {
        dbgAssert(_hasActiveSource);
        _hasActiveSource = false;

        (void)source;
        bytes::Alter{std::move(buffer)};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Input::mapTuid(uint32& mapped, const std::array<uint8, 16>& tuid)
    {
        auto res = _tuidMapFwd.try_emplace(tuid, _tuidMapFwd.size());

        if(!res.second)
        {
            return false;
        }

        mapped = res.first->second;
        dbgAssert(_tuidMapBwd.size() == mapped);

        _tuidMapBwd.emplace_back(tuid);

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Input::unmapTuid(const uint32& mapped, std::array<uint8, 16>& tuid)
    {
        if(_tuidMapBwd.size() <= mapped)
        {
            return false;
        }

        tuid = _tuidMapBwd[mapped];
        return true;
    }
}
