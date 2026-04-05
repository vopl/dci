/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>

#include <memory>
#include <set>
#include <sstream>

#include <QtCore/private/qabstractitemmodel_p.h>
#include <QtCore>
#include <QtWidgets>
#include <QtQml>
#undef interface

#include <dci/poll.hpp>
#include <dci/exception.hpp>
#include <dci/cmt.hpp>
#include <dci/logger.hpp>
#include <dci/sbs.hpp>
#include <dci/idl.hpp>
#include <dci/utils/dbg.hpp>
#include <dci/utils/atScopeExit.hpp>
