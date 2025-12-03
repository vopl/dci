// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "source.hpp"
#include <dci/stiac/link/source.hpp>

namespace dci::stiac::link::impl
{

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Source::Source(Hub4Source* hub, bytes::Alter&& dataAlter, bool doFinalization)
        : _hub(hub)
        , _dataAlter(std::move(dataAlter))
        , _doFinalization(doFinalization)
        , _finalized(false)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Source::~Source()
    {
        finalize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hub4Source* Source::context()
    {
        return _hub;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Source::size() const
    {
        return _dataAlter.size();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Source::read(void* data, uint32 size)
    {
        if(size != _dataAlter.removeTo(data, size))
        {
            fail("low data");
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Source::read(Bytes& data, uint32 size)
    {
        if(size != _dataAlter.removeTo(data, size))
        {
            fail("low data");
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Source::pushPtr(std::type_index typeIndex, const Ptr<void>& v)
    {
        _ptrShareContext.emplace_back(std::move(typeIndex), v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::pair<std::type_index, Ptr<void>> Source::getPtr(uint32 idx)
    {
        if(idx >= _ptrShareContext.size())
        {
            return std::make_pair(std::type_index(typeid(void)), Ptr<void>());
        }

        return _ptrShareContext[idx];
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Source::mapTuid(uint32& mapped, const std::array<uint8, 16>& tuid)
    {
        dbgAssert(_hub);
        return _hub->mapTuid(mapped, tuid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Source::unmapTuid(const uint32& mapped, std::array<uint8, 16>& tuid)
    {
        dbgAssert(_hub);
        return _hub->unmapTuid(mapped, tuid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Source::finalize()
    {
        if(_doFinalization && !finalized())
        {
            _finalized = true;

            dbgAssert(_hub);
            _hub->finalize(
                        himpl::impl2Face<link::Source>(*this),
                        std::move(_dataAlter));
        }

        //_dataAlter.reset();
        _ptrShareContext.clear();

        dbgAssert(!(_dataAlter.size() + _dataAlter.sizeBack()));
        dbgAssert(_ptrShareContext.empty());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Source::finalized() const
    {
        return _finalized;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    [[noreturn]] void Source::fail(const char* cszDetails)
    {
        throw source::Fail(cszDetails);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Source::emplaceLink(link::BasePtr&& link, RemoteId id)
    {
        dbgAssert(_hub);
        if(!_hub->emplaceLink(std::move(link), id))
        {
            fail("bad link id for emplacement");
            return false;
        }

        return true;
    }

}
