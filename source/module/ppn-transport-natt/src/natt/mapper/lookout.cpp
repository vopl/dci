/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "lookout.hpp"
#include "../mapper.hpp"

namespace dci::module::ppn::transport::natt::mapper
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Lookout::Lookout(Mapper* mapper)
        : _mapper{mapper}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Lookout::~Lookout()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    PerformerBlank Lookout::candidate(const apit::Address& internal)
    {
        mapper::PerformerBlank blank;

        for(Service* s : _services)
        {
            mapper::PerformerBlank b = s->candidate(internal);
            if(b._builder && (b._priority > blank._priority || !blank._builder))
            {
                blank = b;
            }
        }

        return blank;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Lookout::activate(Service* s)
    {
        if(_services.insert(s).second)
        {
            LOGI(s->name()<<": activate");
            _mapper->lookoutChanged(this);
            return true;
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Lookout::deactivate(Service* s)
    {
        if(_services.erase(s))
        {
            LOGI(s->name()<<": deactivate");
            _mapper->lookoutChanged(this);
            return true;
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mapper* Lookout::mapper()
    {
        return _mapper;
    }

}
