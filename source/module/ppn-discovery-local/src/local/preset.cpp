/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
