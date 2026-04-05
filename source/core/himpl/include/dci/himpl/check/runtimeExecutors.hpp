/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "../face2Impl.hpp"
#include "../impl2Face.hpp"

#include <cstddef>
#include <typeinfo>
#include <iostream>

namespace dci::himpl::check
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TBase>
    __attribute__((no_sanitize_address))
    //__attribute__((no_sanitize_undefined))
    int runtimeCheckFaceWithBase()
    {
        using F = TFace;
        using FI = typename F::Impl;

        using B = TBase;
        using BI = typename B::Impl;

        //f->b->bi->fi->f == f
        {
            union U
            {
                U():stub{}{}
                ~U(){}
                char stub;
                F f;
            } buf;

            F* f = &buf.f;
            B* b = static_cast<B *>(f);
            BI* bi = face2Impl(b);
            FI* fi = static_cast<FI *>(bi);
            F* f2 = impl2Face<F>(fi);

            if(f != f2)
            {
                std::cerr<<"fail 'f->b->bi->fi->f == f' for F="<<typeid(F).name() << " and B="<<typeid(B).name()<<std::endl;
                abort();
            }
        }

        //f->fi->bi->b->f == f
        {
            union U
            {
                U():stub{}{}
                ~U(){}
                char stub;
                F f;
            } buf;

            F* f = &buf.f;
            FI* fi = face2Impl(f);
            BI* bi = static_cast<BI *>(fi);
            B* b = impl2Face<B>(bi);
            F* f2 = static_cast<F *>(b);

            if(f != f2)
            {
                std::cerr<<"fail 'f->fi->bi->b->f == f' for F="<<typeid(F).name() << " and B="<<typeid(B).name()<<std::endl;
                abort();
            }
        }

        volatile int res = 1;
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace>
    __attribute__((no_sanitize_address))
    //__attribute__((no_sanitize_undefined))
    int runtimeEnumerateBases(std::tuple<>*)
    {
        volatile int res = 1;
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace>
    __attribute__((no_sanitize_address))
    //__attribute__((no_sanitize_undefined))
    int runtimeCheckFace();

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TBase, class... TBases>
    __attribute__((no_sanitize_address))
    //__attribute__((no_sanitize_undefined))
    int runtimeEnumerateBases(std::tuple<TBase, TBases...>*)
    {
        int res = 1;
        //res += checkFace<TBase>();

        res += runtimeCheckFaceWithBase<TFace, TBase>();
        res += runtimeEnumerateBases<TFace>(static_cast<std::tuple<TBases...>*>(nullptr));

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace>
    __attribute__((no_sanitize_address))
    //__attribute__((no_sanitize_undefined))
    int runtimeCheckFace()
    {
        //std::cerr<<"runtimeCheckFace "<<typeid(TFace).name()<<std::endl;


        using F = TFace;
        using FI = typename F::Impl;

        //TODO: вынести в compile time
        if(alignof(F) < alignof(FI))
        {
            std::cerr<<"bad align "<<alignof(F)<<" vs "<<alignof(FI)<<" for F="<<typeid(F).name() << " and FI="<<typeid(FI).name()<<std::endl;
            abort();
        }

        //TODO: вынести в compile time
        struct alignas(alignof(F)) AlignedFi {alignas(alignof(FI)) char _fi[sizeof(FI)];};

        if(!std::is_polymorphic<F>::value && sizeof(F) != sizeof(AlignedFi))
        {
            std::cerr<<"bad simple-face size: "<<sizeof(F)<<" vs "<<sizeof(FI)<<" for F="<<typeid(F).name() << " and FI="<<typeid(FI).name()<<std::endl;
            abort();
        }

        //TODO: вынести в compile time
        if(std::is_polymorphic<F>::value && sizeof(F) - sizeof(void *) != sizeof(AlignedFi))
        {
            std::cerr<<"bad polymorphic-face size: "<<sizeof(F) - sizeof(void *)<<" vs "<< sizeof(FI)<<" for F="<<typeid(F).name() << " and FI="<<typeid(FI).name()<<std::endl;
            abort();
        }

        //f->i->f == f
        {
            union U
            {
                U():stub{}{}
                ~U(){}
                char stub;
                F f;
            } buf;

            F* f = &buf.f;
            FI* fi = face2Impl(f);
            F* f2 = impl2Face<F>(fi);

            if(f != f2)
            {
                std::cerr<<"fail 'f->i->f == f' for F="<<typeid(F).name() << " and FI="<<typeid(FI).name()<<std::endl;
                abort();
            }
        }

        int res = 1;

        res += runtimeEnumerateBases<F>(static_cast<typename F::BasesTuple*>(nullptr));

        return res;
    }

}
