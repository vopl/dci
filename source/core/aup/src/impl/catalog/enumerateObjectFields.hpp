/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/stiac/serialization.hpp>
#include <dci/aup/exception.hpp>
#include <dci/aup/catalog/release.hpp>
#include <dci/aup/catalog/file.hpp>
#include <dci/aup/catalog/object.hpp>
#include <dci/aup/catalog/unit.hpp>

namespace dci::aup::impl::catalog
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline void enumerateObjectFields(auto* object, auto&& f)
    {
        f(object->_dependencies);

        switch(object->type())
        {
        case aup::catalog::Object::Type::file:
            {
                auto* c = aup::catalog::objectPtrCast<aup::catalog::File>(object);
                f(c->_kind);
                f(c->_path);
                f(c->_perms);
                f(stiac::smallIntegral(c->_size));
                f(c->_content);
            }
            break;
        case aup::catalog::Object::Type::unit:
            {
                auto* c = aup::catalog::objectPtrCast<aup::catalog::Unit>(object);
                f(c->_name);
                f(c->_extraAllowed);
            }
            break;
        case aup::catalog::Object::Type::release:
            {
                auto* c = aup::catalog::objectPtrCast<aup::catalog::Release>(object);

                f(c->_srcBranch);
                f(c->_srcRevision);
                f(stiac::smallIntegral(c->_srcMoment));

                f(c->_platformOs);
                f(c->_platformArch);
                f(c->_compiler);
                f(c->_compilerVersion);
                f(c->_compilerOptimization);

                f(stiac::smallIntegral(c->_stability));

                f(c->_vendor);
                f(c->_vendorSign);
                f(c->_signature);
            }
            break;
        default:
            dbgWarn("bad object type");
            throw aup::Exception{"bad object type provided"};
        }
    }
}
