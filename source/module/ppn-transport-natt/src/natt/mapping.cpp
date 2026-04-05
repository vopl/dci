/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "mapping.hpp"
#include "mapper.hpp"

namespace dci::module::ppn::transport::natt
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mapping::Mapping()
        : api::Mapping<>::Opposite{idl::interface::Initializer{}}
    {
        methods()->start() += serviceSol() * [this](const apit::Address& internal, api::Protocol protocol)
        {
            if(internal == _internal && protocol == _protocol)
            {
                return;
            }

            if(_started)
            {
                setExternal({});
                if(_mapper) _mapper->stop(this);
            }
            else
            {
                _started = true;
            }

            _internal = internal;
            _protocol = protocol;
            _started = true;

            if(_mapper) _mapper->start(this);
        };

        methods()->stop() += serviceSol() * [this]
        {
            if(_started)
            {
                setExternal({});
                if(_mapper) _mapper->stop(this);
                _internal = {};
                _protocol = {};
                _started = false;
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mapping::~Mapping()
    {
        setup(nullptr);
        serviceSol().flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Mapping::setup(Mapper* mapper)
    {
        setExternal({});
        if(_mapper && _started) _mapper->stop(this);
        _mapper = mapper;
        if(_mapper && _started) _mapper->start(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const apit::Address& Mapping::internal() const
    {
        return _internal;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Mapping::setExternal(const apit::Address& external)
    {
        if(external == _external)
        {
            return;
        }

        if(!_external.value.empty())
        {
            methods()->unestablished();
        }

        _external = external;

        if(!_external.value.empty())
        {
            methods()->established(_external);
        }
    }

}
