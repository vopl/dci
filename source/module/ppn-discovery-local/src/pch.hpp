/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/host.hpp>
#include <dci/logger.hpp>
#include <dci/utils/atScopeExit.hpp>
#include <dci/poll/waitableTimer.hpp>
#include <dci/stiac.hpp>
#include <dci/utils/uri.hpp>
#include <dci/utils/ip.hpp>
#include "ppn/discovery/local.hpp"

namespace dci::module::ppn::discovery
{
    using namespace dci;

    namespace transport = idl::gen::ppn::transport;
    namespace link      = idl::gen::ppn::node::link;
    namespace node      = idl::gen::ppn::node;
    namespace api       = idl::gen::ppn::discovery::local;
    namespace net       = idl::gen::net;
}
