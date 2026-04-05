/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "mapper.hpp"
#include "mapping.hpp"
#include "mapper/service.hpp"

namespace dci::module::ppn::transport::natt
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mapper::Mapper(dci::host::module::Entry* module)
        : _module{module}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mapper::~Mapper()
    {
        _workers.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    api::Mapping<> Mapper::alloc()
    {
        natt::Mapping* instance = new natt::Mapping;
        instance->involvedChanged() += instance->serviceSol() * [instance](bool v)
        {
            if(!v)
            {
                delete instance;
            }
        };

        instance->setup(this);
        return instance->opposite();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    host::Manager* Mapper::hostManager()
    {
        return _module->manager();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    host::module::StopLocker Mapper::hostModuleStopLocker()
    {
        return _module->stopLocker();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Mapper::lookoutChanged(mapper::Lookout* l)
    {
        (void)l;
        _lookoutsChanged.raise();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Mapper::start(Mapping* mapping)
    {
        auto eres = _workers.emplace(
                        std::piecewise_construct_t{},
                        std::tie(mapping),
                        std::tuple<>{});

        if(!eres.second)
        {
            //already
            return;
        }

        cmt::spawn() += eres.first->second * [this,mapping]
        {
            for(;;)
            {
                try
                {
                    mapper::PerformerPtr performer = performerFor(mapping->internal());

                    utils::AtScopeExit se{[&]
                    {
                        mapping->setExternal({});
                        if(performer)
                        {
                            performer->stop();
                        }
                    }};

                    if(performer->start())
                    {
                        mapping->setExternal(performer->external());
                        while(performer->keepalive())
                        {
                            mapping->setExternal(performer->external());
                        }
                    }
                    else
                    {
                        poll::WaitableTimer<> t{std::chrono::milliseconds{500}};
                        t.start();
                        t.wait();
                    }
                }
                catch(const cmt::task::Stop&)
                {
                    return;
                }
                catch(...)
                {
                    LOGW(exception::toString(std::current_exception()));
                }
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Mapper::stop(Mapping* mapping)
    {
        _workers.erase(mapping);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    mapper::PerformerPtr Mapper::performerFor(const apit::Address& internal)
    {
        mapper::PerformerBlank blank;

        for(;;)
        {
            for(const LookoutPtr& l : _lookouts)
            {
                mapper::PerformerBlank b = l->candidate(internal);
                if(b._builder && (b._priority > blank._priority || !blank._builder))
                {
                    blank = b;
                }
            }

            if(blank._builder)
            {
                break;
            }

            _lookoutsChanged.wait();
        }

        return blank._builder();
    }

}
