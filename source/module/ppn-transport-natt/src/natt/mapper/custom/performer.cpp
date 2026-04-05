/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "performer.hpp"
#include "lookout.hpp"

namespace dci::module::ppn::transport::natt::mapper::custom
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Performer::Performer(Lookout* l, const apit::Address& internal)
        : _l{l}
        , _internal{internal}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Performer::~Performer()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Performer::start()
    {
        return fetch();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Performer::keepalive()
    {
        if(!_l)
        {
            _revision = {};
            _external = {};
            return false;
        }

        if(_revision == _l->revision())
        {
            _l->changed().wait();
        }

        return fetch();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Performer::stop()
    {
        _revision = {};
        _external = {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Performer::fetch()
    {
        if(!_l)
        {
            _revision = {};
            _external = {};
            return false;
        }

        _revision = _l->revision();

        const auto& maps = _l->maps();
        auto iter = maps.find(_internal);
        if(maps.end() == iter)
        {
            _external = {};
            return false;
        }

        _external = iter->second;
        return true;
    }

}
