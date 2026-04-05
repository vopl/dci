/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/primitives.hpp>


namespace dci::aup::applier
{
    enum Result : uint64
    {
        rOk                = 0,

        rCorruptedCatalog  = 0x01,
        rAmbiguousCatalog  = 0x02,
        rIncompleteCatalog = 0x04,
        rIncompleteStorage = 0x08,
        rSomeFailed        = 0x0f,

        rExistsMissings    = 0x10,
        rExistsChanges     = 0x20,
        rExistsExtra       = 0x40,
        rSomeWrong         = 0xf0,

        rFixedMissings     = 0x100,
        rFixedChanges      = 0x200,
        rFixedExtra        = 0x400,
    };
}
