/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "deps.hpp"
#include <dci/utils/dbg.hpp>

namespace dci::idl::gen::executor
{
    using namespace im::ast;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Deps::Deps()
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Deps::~Deps()
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Deps::sname()
    {
        return "deps";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Deps::name()
    {
        return sname();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string Deps::description()
    {
        return "produce dependency content for build system";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    boost::program_options::options_description Deps::options()
    {
        auto local = Text::options();

        local.add_options()
                (
                    (name()+"-target").data(),
                    boost::program_options::value<std::string>()->default_value("cpp"),
                    "specify target language, cpp|ninja"
                )
                (
                    (name()+"-ninja-artifact").data(),
                    boost::program_options::value<std::string>()->default_value("unknown-ninja-artifact"),
                    "specify ninja artifact name"
                )
                ;

        return local;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Deps::run(const im::Storage& ims, const boost::program_options::variables_map& vars)
    {
        if(!Text::run(ims, vars))
        {
            return false;
        }

        std::string target = vars[name()+"-target"].as<std::string>();

        if("cpp" == target)
        {
            _target = Target::cpp;
        }
        else if("ninja" == target)
        {
            _target = Target::ninja;
            _artifact = vars[name()+"-ninja-artifact"].as<std::string>();
        }
        else
        {
            dbgWarn("unknown target provided");
            _target = Target::unknown;
            return false;
        }

        switch(_target)
        {
        case Target::cpp:
            {
                for(const std::string& src : ims.sources())
                {
                    _out<<"#include \""<<src<<"\""<<el;
                }
            }
            break;

        case Target::ninja:
            {
                _out<<_artifact<<":";
                for(const std::string& src : ims.sources())
                {
                    _out<<" \\"<<el<<" "<<src;
                }
                _out<<el;
            }
            break;

        case Target::unknown:
            return false;
        }

        return true;
    }
}

