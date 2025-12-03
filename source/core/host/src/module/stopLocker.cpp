// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/host/module/entry.hpp>
#include <dci/host/module/stopLocker.hpp>

namespace dci::host::module
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StopLocker::~StopLocker()
    {
        if(_e)
        {
            _e->stopLockCounterDec();
            _e = nullptr;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StopLocker::StopLocker()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StopLocker::StopLocker(Entry* e)
        : _e{e}
    {
        dbgAssert(_e);
        if(_e)
        {
            _e->stopLockCounterInc();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StopLocker::StopLocker(StopLocker&& from)
        : _e{std::exchange(from._e, {})}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StopLocker::StopLocker(const StopLocker& from)
        : _e{from._e}
    {
        if(_e)
        {
            _e->stopLockCounterInc();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StopLocker& StopLocker::operator=(StopLocker&& from)
    {
        if(_e == from._e)
        {
            if(from._e)
            {
                from._e->stopLockCounterDec();
                from._e = nullptr;
            }
            return *this;
        }

        if(_e)
        {
            _e->stopLockCounterDec();
        }

        _e = from._e;
        from._e = nullptr;

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StopLocker& StopLocker::operator=(const StopLocker& from)
    {
        if(_e == from._e)
        {
            return *this;
        }

        if(from._e)
        {
            from._e->stopLockCounterInc();
        }

        if(_e)
        {
            _e->stopLockCounterDec();
        }

        _e = from._e;

        return *this;
    }
}
