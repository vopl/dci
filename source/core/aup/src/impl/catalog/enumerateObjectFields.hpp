// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
