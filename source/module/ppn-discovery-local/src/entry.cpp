// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "local/preset.hpp"
#include "local/processScope.hpp"
#include "local/datagramBased/machineScope.hpp"
#include "local/datagramBased/lanScope.hpp"
#include "stiac-support.hpp"

namespace dci::module::ppn::discovery::local
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

                pushServiceId<api::Preset>();
                pushServiceId<api::ProcessScope>();
                pushServiceId<api::MachineScope>();
                pushServiceId<api::LanScope>();
            }
        } manifest_;


        struct Entry
            : public dci::host::module::Entry
        {
            const Manifest& manifest() override
            {
                return manifest_;
            }

            cmt::Future<idl::Interface> createService(idl::ILid ilid) override
            {
                if(auto s = tryCreateService<local::Preset>(ilid)) return cmt::readyFuture(s);
                if(auto s = tryCreateService<local::ProcessScope>(ilid)) return cmt::readyFuture(s);
                if(auto s = tryCreateService<local::datagramBased::MachineScope>(ilid, manager())) return cmt::readyFuture(s);
                if(auto s = tryCreateService<local::datagramBased::LanScope>(ilid, manager())) return cmt::readyFuture(s);

                return dci::host::module::Entry::createService(ilid);
            }
        } entry_;
    }
}

extern "C"
{
    DCI_INTEGRATION_APIDECL_EXPORT dci::host::module::Entry* dciModuleEntry = &dci::module::ppn::discovery::local::entry_;
}
