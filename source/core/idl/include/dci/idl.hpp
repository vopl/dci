/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "idl/contract/id.hpp"
#include "idl/contract/lid.hpp"
#include "idl/contract/lidRegistry.hpp"

#include "idl/contract/arg.hpp"
#include "idl/contract/res.hpp"
#include "idl/contract/resAdapter.hpp"
#include "idl/contract/noReply.hpp"

#include "idl/contract/hmbDescriptor.hpp"
#include "idl/contract/hmDescriptorBase.hpp"
#include "idl/contract/hmDescriptor.hpp"
#include "idl/contract/mdDescriptorBase.hpp"
#include "idl/contract/mdDescriptor.hpp"
#include "idl/contract/mdState.hpp"

#include "idl/contract/wiresBundle.hpp"
#include "idl/contract/wiresBundlesContainer.hpp"

#include "idl/interface.hpp"
#include "idl/interface/side.hpp"
#include "idl/interface/id.hpp"
#include "idl/interface/lid.hpp"
#include "idl/interface/generic.hpp"
#include "idl/interface/implBase.hpp"
#include "idl/interface/stateHolder.hpp"
#include "idl/interface/methods.hpp"
#include "idl/interface/methodsBase.hpp"
#include "idl/interface/exception/methodNotConnected.hpp"
#include "idl/interface/exception/methodNotImplemented.hpp"

#include "idl/iSide.hpp"
#include "idl/iId.hpp"
#include "idl/iLid.hpp"

namespace dci::idl::gen {}
