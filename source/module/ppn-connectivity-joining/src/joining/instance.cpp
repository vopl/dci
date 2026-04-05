/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "instance.hpp"
#include "../joining.hpp"

namespace dci::module::ppn::connectivity::joining
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Instance::Instance(Joining* j, const node::link::Id& id)
        : _j{j}
        , _id{id}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Instance::~Instance()
    {
        _demands.clear();
        _onlines.clear();

        cmt::task::Face f = cmt::task::current();
        if(f.owner() == &_taskOwner)
        {
            f.ownTo(nullptr);
        }

        _taskOwner.stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const node::link::Id Instance::id() const
    {
        return _id;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::demand(api::demand::SatisfactionHolder<>&& demand) const
    {
        auto ires = _demands.emplace(std::piecewise_construct_t{},
                                     std::forward_as_tuple(std::move(demand)),
                                     std::tuple{});
        if(ires.second)
        {
            const api::demand::SatisfactionHolder<>& d2 = ires.first->first;
            sbs::Owner& sol = ires.first->second;

            d2.involvedChanged() += sol * [this,d2=d2.weak()](bool v)
            {
                if(!v)
                {
                    if(_demands.erase(d2))
                    {
                        if(_demands.empty())
                        {
                            _j->instanceEmpty(_id);
                        }
                        else
                        {
                            activate();
                        }
                    }
                }
            };

            activate();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::discovered() const
    {
        if(!_hasAddress)
        {
            _hasAddress = true;
            activate();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::joined(const node::link::Remote<>& r) const
    {
        auto ires = _onlines.emplace(std::piecewise_construct_t{},
                                     std::tie(r),
                                     std::tuple{});

        if(ires.second)
        {
            sbs::Owner& sol = ires.first->second;

            r.involvedChanged() += sol * [this,r=r.weak()](bool v)
            {
                if(!v)
                {
                    if(_onlines.erase(r))
                    {
                        activate();
                    }
                }
            };

            r->closed() += sol * [this,r=r.weak()]()
            {
                if(_onlines.erase(r))
                {
                    activate();
                }
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::activate() const
    {
        if(_taskOwner.empty())
        {
            cmt::spawn() += _taskOwner * [this]
            {
                try
                {
                    worker();
                    return;
                }
                catch(const cmt::task::Stop&)
                {
                    return;
                }
                catch(...)
                {
                    //LOGW(exception::toString(std::current_exception()));
                    if(_onlines.empty())
                    {
                        _demands.clear();
                        _j->instanceEmpty(_id);
                        return;
                    }
                }
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::worker() const
    {
        if(!_onlines.empty())
        {
            return;
        }

        if(!_hasAddress)
        {
            return;
        }

        auto [remote, address, rating] = _j->fetchRdbRecord(_id).value();

        if(remote)
        {
            joined(remote);
            return;
        }

        if(address.value.empty())
        {
            _hasAddress = false;
            return;
        }

        if(0 > rating)
        {
            return;
        }

        remote = _j->join(_id, std::move(address)).value();

        if(remote)
        {
            if(remote->id().value() == _id)
            {
                joined(remote);
            }
        }
    }
}
