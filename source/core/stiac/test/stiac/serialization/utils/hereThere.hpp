/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once
#include <dci/stiac/serialization.hpp>
#include <dci/stiac/link/sink.hpp>
#include <dci/stiac/link/source.hpp>
#include <dci/idl/iId.hpp>
#include <dci/bytes.hpp>
#include <vector>

namespace utils
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace moc
    {
        //global
        inline std::map<std::array<std::uint8_t, 16>, std::uint32_t> tuidMappingContext_map4Out;
        inline std::map<std::uint32_t, std::array<std::uint8_t, 16>> tuidMappingContext_map4In;

        struct Hub
            : public dci::stiac::link::Hub4Sink
            , public dci::stiac::link::Hub4Source
        {

            //Hub4Sink
            virtual dci::stiac::link::LocalId emplaceLink(dci::stiac::link::BasePtr&& /*link*/) override
            {
                dbgFatal("not impl");
                return {};
            }

            virtual void finalize(dci::stiac::link::Sink& /*sink*/, dci::bytes::Alter&& buffer) override
            {
                dci::bytes::Alter{std::move(buffer)};
            }

            virtual std::pair<dci::uint32, bool> mapTuid(const std::array<dci::uint8, 16>& tuid) override
            {
                auto iterAndPresense = tuidMappingContext_map4Out.try_emplace(tuid, tuidMappingContext_map4Out.size());
                return std::make_pair(iterAndPresense.first->second, iterAndPresense.second);
            }

            //Hub4Source
            virtual bool emplaceLink(dci::stiac::link::BasePtr&& /*link*/, dci::stiac::link::RemoteId /*remoteId*/) override
            {
                dbgFatal("not impl");
                return {};
            }

            virtual void finalize(dci::stiac::link::Source& /*source*/, dci::bytes::Alter&& buffer) override
            {
                dci::bytes::Alter{std::move(buffer)};
            }

            virtual bool mapTuid(dci::uint32& mapped, const std::array<dci::uint8, 16>& tuid) override
            {
                return tuidMappingContext_map4In.try_emplace(mapped, tuid).second;
            }

            virtual bool unmapTuid(const dci::uint32& mapped, std::array<dci::uint8, 16>& tuid) override
            {
                auto iter = tuidMappingContext_map4In.find(mapped);
                if(tuidMappingContext_map4In.end() == iter)
                {
                    return false;
                }

                tuid = iter->second;
                return true;
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class V, class Comparator>
    void hereThere(V&& v, const Comparator& comparator)
    {
        dci::Bytes buffer;
        moc::Hub hub;

        std::decay_t<V> vSrc = v;
        std::decay_t<V> vDst {};

        dci::stiac::link::Sink{&hub, buffer.begin()} << std::forward<decltype(v)>(v);
        dci::stiac::link::Source{&hub, buffer.begin()} >> vDst;

        if(!buffer.empty())
        {
            throw std::runtime_error("Buffer must be empty");
        }

        comparator(vSrc, vDst);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class V, class Comparator>
    void hereThere2(V&& v, const Comparator& comparator)
    {
        hereThere(v,            comparator);
        hereThere(std::move(v), comparator);
    }
}
