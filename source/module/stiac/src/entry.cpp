// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "protocol.hpp"

#include "stiac-stiac-support.hpp"

namespace dci::module::stiac
{
    namespace
    {
        struct Manifest
            : public host::module::Manifest
        {
            Manifest()
            {
                _valid = true;
                _name = dciModuleName;
                _mainBinary = dciUnitTargetFile;

                pushServiceId<api::Protocol>();
            }
        } manifest_;

        struct Entry
            : public host::module::Entry
        {
            const Manifest& manifest() override
            {
                return manifest_;
            }

            cmt::Future<idl::Interface> createService(idl::ILid ilid) override
            {
                if(auto s = tryCreateService<Protocol>(ilid)) return cmt::readyFuture(s);
                return dci::host::module::Entry::createService(ilid);
            }
        } entry_;
    }
}

extern "C"
{
    DCI_INTEGRATION_APIDECL_EXPORT dci::host::module::Entry* dciModuleEntry = &dci::module::stiac::entry_;
}
