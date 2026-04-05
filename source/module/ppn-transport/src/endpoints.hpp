/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "pch.hpp"
#include "endpoint.hpp"

namespace dci::module::ppn::transport
{
    template <class Iface>
    class Endpoints
        : private List<std::unique_ptr<Endpoint<Iface>>>
    {
    public:
        using LowerLayer = Endpoint<Iface>;
        using LowerLayerPtr = std::unique_ptr<LowerLayer>;
        using LowerLayers = List<LowerLayerPtr>;

        LowerLayer& add(Iface&& instance)
        {
            const LowerLayerPtr& llp = this->emplace_back(std::make_unique<LowerLayer>());
            llp->subscribe(std::move(instance));
            return *llp;
        }

        void del(const Iface& instance)
        {
            for(std::size_t i(0); i<this->size(); ++i)
            {
                if((*this)[i]->_instance == instance)
                {
                    (*this)[i]->unsubscribe();
                    this->erase(this->begin() + static_cast<std::ptrdiff_t>(i));
                    break;
                }
            }
        }

        using LowerLayers::empty;
        using LowerLayers::begin;
        using LowerLayers::cbegin;
        using LowerLayers::end;
        using LowerLayers::cend;
        using LowerLayers::clear;
    private:
    };
}
