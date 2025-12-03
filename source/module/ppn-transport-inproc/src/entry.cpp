// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "connector.hpp"
#include "acceptor.hpp"
#include "channelBridge/pumper.hpp"

namespace dci::module::ppn::transport::inproc
{
    namespace
    {
        struct Manifest
            : public dci::host::module::Manifest
        {
            Manifest()
            {
                _valid = true;
                _name = dciModuleName;
                _mainBinary = dciUnitTargetFile;

                pushServiceId<api::Connector>();
                pushServiceId<api::Acceptor>();
            }
        } manifest_;


        struct Entry
            : public dci::host::module::Entry
        {
            const Manifest& manifest() override
            {
                return manifest_;
            }

            bool start(dci::host::Manager* manager) override
            {
                if(!dci::host::module::Entry::start(manager))
                {
                    return false;
                }

                dbgAssert(!channelBridge::g_pumperPtr);
                if(!channelBridge::g_pumperPtr)
                {
                    channelBridge::g_pumperPtr = new channelBridge::Pumper;
                }

                return true;
            }

            bool stop() override
            {
                dbgAssert(channelBridge::g_pumperPtr);
                if(channelBridge::g_pumperPtr)
                {
                    delete channelBridge::g_pumperPtr;
                    channelBridge::g_pumperPtr = nullptr;
                }

                return dci::host::module::Entry::stop();
            }

            cmt::Future<idl::Interface> createService(idl::ILid ilid) override
            {
                if(auto s = tryCreateService<Connector>(ilid)) return cmt::readyFuture(s);
                if(auto s = tryCreateService<Acceptor>(ilid)) return cmt::readyFuture(s);

                return dci::host::module::Entry::createService(ilid);
            }
        } entry_;
    }
}

extern "C"
{
    DCI_INTEGRATION_APIDECL_EXPORT dci::host::module::Entry* dciModuleEntry = &dci::module::ppn::transport::inproc::entry_;
}
