// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "preset.hpp"

namespace dci::module::ppn::discovery::local
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Preset::Preset()
    {
        //in configure(Config) -> void;
        methods()->configure() += serviceSol() * [this](idl::gen::Config&& config)
        {
            for(auto& p : config.children)
            {
                std::string addr = std::get<0>(p);

                if(!utils::uri::valid(addr))
                {
                    return cmt::readyFuture<None>(exception::buildInstance<api::Error>("bad address value in config for preset: "+addr));
                }

                declareImpl(Entry{{}, {std::move(addr)}});
            }

            return cmt::readyFuture(None{});
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Preset::~Preset()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Preset::started()
    {
        emitEntries(entriesOut(), true);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Preset::declared(const Entry& entry)
    {
        emitEntry(entry, true);
    }
}
