// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "www.hpp"
#include "www-stiac-support.hpp"
#include "factory.hpp"
#include "tls.hpp"
#include "http/client/cookies.hpp"
#include "agent.hpp"
#include "channelSoftClosing.hpp"

namespace dci::module::www
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

                pushServiceId<api::Factory>();
                pushServiceId<api::Tls>();
                pushServiceId<api::http::client::Cookies>();
                pushServiceId<api::Agent>();
            }
        } manifest_;

        struct Entry
            : public dci::host::module::Entry
        {
            const Manifest& manifest() override
            {
                return manifest_;
            }

            bool start(host::Manager* manager) override
            {
                ChannelSoftClosing::moduleStarted();
                return dci::host::module::Entry::start(manager);
            }

            cmt::Future<> stopRequest() override
            {
                ChannelSoftClosing::moduleStopRequested();
                return dci::host::module::Entry::stopRequest();
            }

            bool stop() override
            {
                ChannelSoftClosing::moduleStopped();
                return dci::host::module::Entry::stop();
            }

            dci::cmt::Future<dci::idl::Interface> createService(dci::idl::ILid ilid) override
            {
                if(auto s = tryCreateService<Factory>(ilid, manager())) return cmt::readyFuture(s);
                if(auto s = tryCreateService<Tls>(ilid)) return cmt::readyFuture(s);
                if(auto s = tryCreateService<http::client::Cookies>(ilid)) return cmt::readyFuture(s);
                if(auto s = tryCreateService<Agent>(ilid, manager())) return cmt::readyFuture(s);
                return dci::host::module::Entry::createService(ilid);
            }
        } entry_;
    }
}

extern "C"
{
    DCI_INTEGRATION_APIDECL_EXPORT dci::host::module::Entry* dciModuleEntry = &dci::module::www::entry_;
}
