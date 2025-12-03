// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/host/module/manifest.hpp>
#include <dci/logger.hpp>
#include <dci/exception/toString.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

namespace dci::host::module
{
    using namespace boost::property_tree;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Manifest::reset()
    {
        _valid = false;
        _name.clear();
        _serviceIds.clear();
    }

    namespace
    {
        template <class In>
        bool fromConf(Manifest& target, In& in)
        {
            target.reset();

            ptree pt, npt;
            try
            {
                read_info(in, pt);

                target._name = pt.get<std::string>("name");
                target._mainBinary = pt.get<std::string>("mainBinary");

                target._serviceIds.clear();
                for(auto& v: pt.get_child_optional("serviceIds").get_value_or(npt))
                {
                    target._serviceIds.emplace_back();
                    Manifest::ServiceId& id = target._serviceIds.back();
                    if(!id._iid.fromText(v.first))
                    {
                       throw std::runtime_error("malformed service id: "+v.first);
                    }
                    id._alias = v.second.data();
                }

                target._valid = true;
            }
            catch(...)
            {
                LOGE("unable to parse module manifest: "<<exception::currentToString());
                return false;
            }

            return true;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Manifest::fromConf(const std::string& conf)
    {
        std::istringstream iss(conf);
        return module::fromConf(*this, iss);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Manifest::fromConfFile(const std::string& path)
    {
        return module::fromConf(*this, path);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Manifest::toConf() const
    {
        if(!_valid)
        {
            LOGE("unable to make module manifest conf: invalid manifest");
            return std::string();
        }

        ptree pt;
        {
            pt.add("name", _name);
            pt.add("mainBinary", _mainBinary);

            if(!_serviceIds.empty())
            {
                ptree vals;
                for(const auto& v : _serviceIds)
                {
                    vals.push_back(std::make_pair(v._iid.toText(), ptree(v._alias)));
                }
                pt.push_back(std::make_pair("serviceIds", vals));
            }
        }

        std::stringstream ss;
        write_info(ss, pt);
        return ss.str();
    }
}
