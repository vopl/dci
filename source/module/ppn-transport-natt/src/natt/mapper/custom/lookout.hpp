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
#include "../lookout.hpp"
#include "../service.hpp"
#include "../performer.hpp"

namespace dci::module::ppn::transport::natt::mapper::custom
{
    class Lookout
        : public mapper::Lookout
        , private mapper::Service
    {
    public:
        using Maps = std::multimap<apit::Address, apit::Address>;

    public:
        Lookout(Mapper* mapper);
        ~Lookout() override;

        void map(const apit::Address& i, const apit::Address& e);
        void unmap(const apit::Address& i, const apit::Address& e);

        PerformerBlank candidate(const apit::Address& internal) override;

        const Maps& maps() const;
        std::size_t revision() const;
        cmt::Waitable& changed();

    private:
        Maps        _maps;
        std::size_t _revision{};
        cmt::Pulser _changed;
    };
}
