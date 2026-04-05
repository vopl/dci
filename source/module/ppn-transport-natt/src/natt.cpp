/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "natt.hpp"
#include "natt/mapper/pmpPcp/lookout.hpp"
#include "natt/mapper/igdp/lookout.hpp"
#include "natt/mapper/awsEc2/lookout.hpp"
#include "natt/mapper/custom/lookout.hpp"

namespace dci::module::ppn::transport
{
    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        bool parseBool(const String& param)
        {
            static const std::regex t("^(t|true|on|enable|allow|1)$", std::regex_constants::icase | std::regex::optimize);
            return std::regex_match(param, t);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Natt::Natt(host::module::Entry* module)
        : apit::Natt<>::Opposite{idl::interface::Initializer{}}
        , _mapper{module}
    {
        methods()->configure() += serviceSol() * [this](idl::gen::Config&& c)
        {
            config::ptree conf = config::cnvt(std::move(c));

            {
                boost::optional<String> opt = conf.get_optional<String>("pmp");
                bool usePmp = (!opt || parseBool(*opt));

                opt = conf.get_optional<String>("pcp");
                bool usePcp = (!opt || parseBool(*opt));

                if(usePmp || usePcp)
                {
                    _mapper.add<natt::mapper::pmpPcp::Lookout>(usePmp, usePcp);
                }

                opt = conf.get_optional<String>("igdp");
                if(!opt || parseBool(*opt))
                {
                    _mapper.add<natt::mapper::igdp::Lookout>();
                }

                opt = conf.get_optional<String>("awsEc2");
                if(!opt || parseBool(*opt))
                {
                    _mapper.add<natt::mapper::awsEc2::Lookout>();
                }
            }

            {
                auto opt = conf.get_child_optional("custom");

                if(opt)
                {
                    natt::mapper::custom::Lookout* cust = _mapper.add<natt::mapper::custom::Lookout>();
                    for(const auto&[i, e] : *opt)
                    {
                        cust->map(apit::Address{i}, apit::Address{e.get_value("")});
                    }
                }
            }

            return cmt::readyFuture(None{});
        };

        methods()->mapping() += serviceSol() * [this]
        {
            return cmt::readyFuture(_mapper.alloc());
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Natt::~Natt()
    {
        serviceSol().flush();
    }
}
