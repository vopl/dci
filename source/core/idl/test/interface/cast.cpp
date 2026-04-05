/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/test.hpp>
#include "interface/cast.hpp"

using namespace dci::idl;
using namespace dci::idl::gen;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(idl, interface_cast)
{
    {
        h1::Iface1<> i1;
        EXPECT_FALSE(i1);
    }

    {
        h1::Iface1<>::Opposite i1;
        EXPECT_FALSE(i1);
    }

    {
        h1::Iface1<> i1t;

        {
            h1::Iface1<> v(i1t);
            EXPECT_FALSE(v);
        }

        {
            h1::Iface2<> v(i1t);
            EXPECT_FALSE(v);
        }

        {
            h1::Iface1<> v(std::move(i1t));
            EXPECT_FALSE(v);
        }

        {
            h1::Iface2<> v(std::move(i1t));
            EXPECT_FALSE(v);
        }

        {
            h1::Iface1<> v;
            v = i1t;
            EXPECT_FALSE(v);
        }

        {
            h1::Iface2<> v;
            v = i1t;
            EXPECT_FALSE(v);
        }

        {
            h1::Iface1<> v;
            v = std::move(i1t);
            EXPECT_FALSE(v);
        }

        {
            h1::Iface2<> v;
            v = std::move(i1t);
            EXPECT_FALSE(v);
        }
    }

    {
        h1::Iface1<> i1;

        i1.init();
//        EXPECT_TRUE(i1);
//        EXPECT_EQ(i1.id(), h1::Iface1<>::id());
//        EXPECT_EQ(i1.lid(), h1::Iface1<>::lid());
//        EXPECT_EQ(i1.mdId(), h1::Iface1<>::id());
//        EXPECT_EQ(i1.mdLid(), h1::Iface1<>::lid());

//        {
//            h1::Iface1<> v;
//            v = i1;
//            EXPECT_TRUE(v);

//            EXPECT_EQ(v.iid(), h1::Iface1<>::id());
//            EXPECT_EQ(v.ilid(), h1::Iface1<>::lid());
//            EXPECT_EQ(v.mdiid(), h1::Iface1<>::id());
//            EXPECT_EQ(v.mdilid(), h1::Iface1<>::lid());
//        }

        {
            h1::Iface2<> v;
            v = i1;
            EXPECT_FALSE(v);

            v.init();
            i1 = v;
            EXPECT_FALSE(i1);
        }
    }

    {
        h1::Iface1<> i1;

        Interface i(i1);
        EXPECT_FALSE(i);

        i = i1;
        EXPECT_FALSE(i);

    }

    {
        h1::Iface1<> i1;
        i1.init();

        Interface i(i1);
        EXPECT_TRUE(i);
        EXPECT_EQ(i.mdId(), h1::Iface1<>::id());
        EXPECT_EQ(i.mdLid(), h1::Iface1<>::lid());

        i = i1;
        EXPECT_TRUE(i);
        EXPECT_EQ(i.mdId(), h1::Iface1<>::id());
        EXPECT_EQ(i.mdLid(), h1::Iface1<>::lid());

        h1::Iface1<> i12(i);
        EXPECT_TRUE(i12);
        EXPECT_EQ(i12.mdId(), h1::Iface1<>::id());
        EXPECT_EQ(i12.mdLid(), h1::Iface1<>::lid());

        i12 = i;
        EXPECT_TRUE(i12);
        EXPECT_EQ(i12.mdId(), h1::Iface1<>::id());
        EXPECT_EQ(i12.mdLid(), h1::Iface1<>::lid());
    }

    //касты с иерархиями
    {
        h2::Checkbox<> cb;
        cb.init();

        EXPECT_TRUE(cb);
        EXPECT_EQ(cb.id(), h2::Checkbox<>::id());
        EXPECT_EQ(cb.lid(), h2::Checkbox<>::lid());
        EXPECT_EQ(cb.mdId(), h2::Checkbox<>::id());
        EXPECT_EQ(cb.mdLid(), h2::Checkbox<>::lid());

        h2::Button<> b(cb);
        EXPECT_TRUE(b);
        EXPECT_EQ(b.id(), h2::Button<>::id());
        EXPECT_EQ(b.lid(), h2::Button<>::lid());
        EXPECT_EQ(b.mdId(), h2::Checkbox<>::id());
        EXPECT_EQ(b.mdLid(), h2::Checkbox<>::lid());

        h2::Clickable<> c;
        c = cb;
        EXPECT_TRUE(c);
        EXPECT_EQ(c.id(), h2::Clickable<>::id());
        EXPECT_EQ(c.lid(), h2::Clickable<>::lid());
        EXPECT_EQ(c.mdId(), h2::Checkbox<>::id());
        EXPECT_EQ(c.mdLid(), h2::Checkbox<>::lid());

        h2::Rectangle<> r;
        r = b;
        EXPECT_TRUE(r);
        EXPECT_EQ(r.id(), h2::Rectangle<>::id());
        EXPECT_EQ(r.lid(), h2::Rectangle<>::lid());
        EXPECT_EQ(r.mdId(), h2::Checkbox<>::id());
        EXPECT_EQ(r.mdLid(), h2::Checkbox<>::lid());

        h2::Object<> o(r);
        EXPECT_TRUE(o);
        EXPECT_EQ(o.id(), h2::Object<>::id());
        EXPECT_EQ(o.lid(), h2::Object<>::lid());
        EXPECT_EQ(o.mdId(), h2::Checkbox<>::id());
        EXPECT_EQ(o.mdLid(), h2::Checkbox<>::lid());

        Interface i(o);
        EXPECT_TRUE(i);
        EXPECT_EQ(i.mdId(), h2::Checkbox<>::id());
        EXPECT_EQ(i.mdLid(), h2::Checkbox<>::lid());

        h2::Rectangle<> r2;
        r2 = i;
        EXPECT_TRUE(r);
        EXPECT_EQ(r2.id(), h2::Rectangle<>::id());
        EXPECT_EQ(r2.lid(), h2::Rectangle<>::lid());
        EXPECT_EQ(r2.mdId(), h2::Checkbox<>::id());
        EXPECT_EQ(r2.mdLid(), h2::Checkbox<>::lid());


        h2::Clickable<> c2(i);
        EXPECT_TRUE(c2);
        EXPECT_EQ(c2.id(), h2::Clickable<>::id());
        EXPECT_EQ(c2.lid(), h2::Clickable<>::lid());
        EXPECT_EQ(c2.mdId(), h2::Checkbox<>::id());
        EXPECT_EQ(c2.mdLid(), h2::Checkbox<>::lid());

        h2::Button<> b2(c2);
        EXPECT_TRUE(b2);
        EXPECT_EQ(b2.id(), h2::Button<>::id());
        EXPECT_EQ(b2.lid(), h2::Button<>::lid());
        EXPECT_EQ(b2.mdId(), h2::Checkbox<>::id());
        EXPECT_EQ(b2.mdLid(), h2::Checkbox<>::lid());

        h2::Checkbox<> cb2(i);
        EXPECT_TRUE(cb2);
        EXPECT_EQ(cb2.id(), h2::Checkbox<>::id());
        EXPECT_EQ(cb2.lid(), h2::Checkbox<>::lid());
        EXPECT_EQ(cb2.mdId(), h2::Checkbox<>::id());
        EXPECT_EQ(cb2.mdLid(), h2::Checkbox<>::lid());

        EXPECT_EQ(cb, cb);
        EXPECT_EQ(cb, b);
        EXPECT_EQ(cb, c);
        EXPECT_EQ(cb, r);
        EXPECT_EQ(cb, o);
        EXPECT_EQ(cb, i);

        EXPECT_EQ(b, cb);
        EXPECT_EQ(b, b);
        EXPECT_EQ(b, c);
        EXPECT_EQ(b, r);
        EXPECT_EQ(b, o);
        EXPECT_EQ(b, i);

        EXPECT_EQ(c, cb);
        EXPECT_EQ(c, b);
        EXPECT_EQ(c, c);
        EXPECT_EQ(c, r);
        EXPECT_EQ(c, o);
        EXPECT_EQ(c, i);

        EXPECT_EQ(r, cb);
        EXPECT_EQ(r, b);
        EXPECT_EQ(r, c);
        EXPECT_EQ(r, r);
        EXPECT_EQ(r, o);
        EXPECT_EQ(r, i);

        EXPECT_EQ(o, cb);
        EXPECT_EQ(o, b);
        EXPECT_EQ(o, c);
        EXPECT_EQ(o, r);
        EXPECT_EQ(o, o);
        EXPECT_EQ(o, i);

        EXPECT_EQ(i, cb);
        EXPECT_EQ(i, b);
        EXPECT_EQ(i, c);
        EXPECT_EQ(i, r);
        EXPECT_EQ(i, o);
        EXPECT_EQ(i, i);
    }

}
