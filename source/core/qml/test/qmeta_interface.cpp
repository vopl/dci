/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/test.hpp>
#include <dci/idl/interface.hpp>
#include <dci/qml/qmeta.hpp>
#include "idl-victim.hpp"
#include <dci/qml/qmeta.hpp>
#include <QtCore/private/qmetaobject_p.h>
#include <QtCore/private/qmetaobjectbuilder_p.h>

using namespace dci;
using namespace dci::idl;
using namespace dci::idl::gen;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(qml, qmeta_interface)
{
    using T = a::b::c::I1<>;
    auto src = dci::qml::qmeta::Object<T>{};

    QMetaObjectBuilder{src.qt()}.toMetaObject();// дымный тест что это не упадет

    const QMetaObject* o = src.qt();

    EXPECT_EQ(0, strcmp(o->className(), "dci::idl::gen::a::b::c::I1<dci::idl::interface::Side::primary>"));
    EXPECT_EQ(nullptr, o->superClass());
    EXPECT_EQ(0, o->constructorCount());

    EXPECT_EQ(3, o->methodCount());
    {
        QMetaMethod m = o->method(0);
        EXPECT_EQ(m.name(), QByteArray{"m0"});
        EXPECT_EQ(m.returnMetaType(), (QMetaType::fromType<void>()));
        EXPECT_EQ(m.parameterCount(), 1);
        EXPECT_EQ(m.parameterMetaType(0), (QMetaType::fromType<int32>()));
        EXPECT_EQ(m.parameterNames(), QList<QByteArray>{"arg0"});
        EXPECT_EQ(0, strcmp(m.tag(), ""));
        EXPECT_EQ(m.access(), QMetaMethod::Access::Public);
        EXPECT_EQ(m.methodType(), QMetaMethod::MethodType::Method);
        EXPECT_EQ(m.attributes(), 0);
        EXPECT_EQ(m.revision(), 0);
    }
    {
        QMetaMethod m = o->method(1);
        EXPECT_EQ(m.name(), QByteArray{"m1"});
        EXPECT_EQ(m.returnMetaType(), (QMetaType::fromType<cmt::Future<None>>()));
        EXPECT_EQ(m.parameterCount(), 2);
        EXPECT_EQ(m.parameterMetaType(0), (QMetaType::fromType<int32>()));
        EXPECT_EQ(m.parameterMetaType(1), (QMetaType::fromType<dci::String>()));
        EXPECT_EQ(m.parameterNames(), (QList<QByteArray>{{"param1"}, {"param2"}}));
        EXPECT_EQ(0, strcmp(m.tag(), ""));
        EXPECT_EQ(m.access(), QMetaMethod::Access::Public);
        EXPECT_EQ(m.methodType(), QMetaMethod::MethodType::Method);
        EXPECT_EQ(m.attributes(), 0);
        EXPECT_EQ(m.revision(), 0);
    }
    {
        QMetaMethod m = o->method(2);
        EXPECT_EQ(m.name(), QByteArray{"m2"});
        EXPECT_EQ(m.returnMetaType(), (QMetaType::fromType<cmt::Future<dci::String>>()));
        EXPECT_EQ(m.parameterCount(), 3);
        EXPECT_EQ(m.parameterMetaType(0), (QMetaType::fromType<dci::Array<int32, 2>>()));
        EXPECT_EQ(m.parameterMetaType(1), (QMetaType::fromType<uint8>()));
        EXPECT_EQ(m.parameterMetaType(2), (QMetaType::fromType<dci::idl::gen::a::b::c::S1>()));
        EXPECT_EQ(m.parameterNames(), (QList<QByteArray>{{"param3"}, {"arg1"}, {"arg2"}}));
        EXPECT_EQ(0, strcmp(m.tag(), ""));
        EXPECT_EQ(m.access(), QMetaMethod::Access::Public);
        EXPECT_EQ(m.methodType(), QMetaMethod::MethodType::Method);
        EXPECT_EQ(m.attributes(), 0);
        EXPECT_EQ(m.revision(), 0);
    }


    EXPECT_EQ(3, o->propertyCount());
    {
        QMetaProperty p = o->property(0);
        EXPECT_EQ(0, strcmp(p.name(), "s0"));
        EXPECT_EQ(0, (strcmp(p.typeName(), p.metaType().name())));
        EXPECT_EQ(p.metaType(), QMetaType(static_cast<int>(o->d.data[14+p.propertyIndex()*5+1])));
        EXPECT_EQ(p.metaType(), (QMetaType::fromType<dci::sbs::Signal<void, int32>>()));
        EXPECT_TRUE(p.isReadable());
        EXPECT_FALSE(p.isWritable());
        EXPECT_FALSE(p.isResettable());
        EXPECT_FALSE(p.isDesignable());
        EXPECT_TRUE(p.isScriptable());
        EXPECT_FALSE(p.isStored());
        EXPECT_FALSE(p.isUser());
        EXPECT_TRUE(p.isConstant());
        EXPECT_TRUE(p.isFinal());
        EXPECT_FALSE(p.isRequired());
        EXPECT_FALSE(p.isBindable());
        EXPECT_FALSE(p.isFlagType());
        EXPECT_FALSE(p.isEnumType());
        EXPECT_FALSE(p.hasNotifySignal());
        EXPECT_FALSE(p.hasStdCppSet());
        EXPECT_FALSE(p.isAlias());
    }
    {
        QMetaProperty p = o->property(1);
        EXPECT_EQ(0, strcmp(p.name(), "s1"));
        EXPECT_EQ(0, (strcmp(p.typeName(), p.metaType().name())));
        EXPECT_EQ(p.metaType(), QMetaType(static_cast<int>(o->d.data[14+p.propertyIndex()*5+1])));
        EXPECT_EQ(p.metaType(), (QMetaType::fromType<dci::sbs::Signal<cmt::Future<None>, int32, dci::String>>()));
        EXPECT_TRUE(p.isReadable());
        EXPECT_FALSE(p.isWritable());
        EXPECT_FALSE(p.isResettable());
        EXPECT_FALSE(p.isDesignable());
        EXPECT_TRUE(p.isScriptable());
        EXPECT_FALSE(p.isStored());
        EXPECT_FALSE(p.isUser());
        EXPECT_TRUE(p.isConstant());
        EXPECT_TRUE(p.isFinal());
        EXPECT_FALSE(p.isRequired());
        EXPECT_FALSE(p.isBindable());
        EXPECT_FALSE(p.isFlagType());
        EXPECT_FALSE(p.isEnumType());
        EXPECT_FALSE(p.hasNotifySignal());
        EXPECT_FALSE(p.hasStdCppSet());
        EXPECT_FALSE(p.isAlias());
    }
    {
        QMetaProperty p = o->property(2);
        EXPECT_EQ(0, strcmp(p.name(), "s2"));
        EXPECT_EQ(0, (strcmp(p.typeName(), p.metaType().name())));
        EXPECT_EQ(p.metaType(), QMetaType(static_cast<int>(o->d.data[14+p.propertyIndex()*5+1])));
        EXPECT_EQ(p.metaType(), (QMetaType::fromType<dci::sbs::Signal<cmt::Future<dci::String>, dci::Array<int32, 2>, uint8, dci::idl::gen::a::b::c::S1>>()));
        EXPECT_TRUE(p.isReadable());
        EXPECT_FALSE(p.isWritable());
        EXPECT_FALSE(p.isResettable());
        EXPECT_FALSE(p.isDesignable());
        EXPECT_TRUE(p.isScriptable());
        EXPECT_FALSE(p.isStored());
        EXPECT_FALSE(p.isUser());
        EXPECT_TRUE(p.isConstant());
        EXPECT_TRUE(p.isFinal());
        EXPECT_FALSE(p.isRequired());
        EXPECT_FALSE(p.isBindable());
        EXPECT_FALSE(p.isFlagType());
        EXPECT_FALSE(p.isEnumType());
        EXPECT_FALSE(p.hasNotifySignal());
        EXPECT_FALSE(p.hasStdCppSet());
        EXPECT_FALSE(p.isAlias());
    }
}
