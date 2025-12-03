// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/host/module/entry.hpp>
#include <dci/host/module/stopLocker.hpp>
#include <dci/host/exception.hpp>

namespace dci::host::module
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Entry::Entry()
        : _stopLock{}
    {
        _stopLock.resolveValue();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Entry::~Entry()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Entry::load()
    {
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Entry::unload()
    {
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Entry::start(Manager* manager)
    {
        dbgAssert(!_manager);
        _manager = manager;
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<> Entry::stopRequest()
    {
        return _stopLock.future();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Entry::stop()
    {
        _manager = nullptr;
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<idl::Interface> Entry::createService(idl::ILid ilid)
    {
        (void)ilid;
        return cmt::readyFuture<idl::Interface>(std::make_exception_ptr(exception::UnableToCreateService("not implemented")));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Manager* Entry::manager() const
    {
        return _manager;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StopLocker Entry::stopLocker()
    {
        return StopLocker{this};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Entry::stopLockCounterInc()
    {
        ++_stopLockCounter;

        if(1 == _stopLockCounter)
        {
            _stopLock = cmt::Promise<>{};
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Entry::stopLockCounterDec()
    {
        dbgAssert(_stopLockCounter > 0);

        if(_stopLockCounter > 0)
        {
            --_stopLockCounter;

            if(!_stopLockCounter)
            {
                _stopLock.resolveValue();
            }
        }
    }
}
