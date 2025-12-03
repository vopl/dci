// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
