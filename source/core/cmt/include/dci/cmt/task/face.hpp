/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/cmt/api.hpp>
#include <dci/himpl.hpp>
#include <dci/cmt/implMetaInfo.hpp>
#include "../promise.hpp"
#include "key.hpp"
#include "state.hpp"

namespace dci::cmt::task
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Owner;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class API_DCI_CMT Face
        : public himpl::FaceLayout<Face, impl::task::Face>
    {
    public:
        Face();
        Face(const Face&);
        Face(Face&&);
        ~Face();

        void operator=(const Face&);
        void operator=(Face&&);

    public:
        Key key() const;
        bool isCurrent() const;
        State state() const;

    public:
        void stop(bool throwSelf = false);
        bool stopRequested() const;

        void ownTo(Owner* owner);
        Owner* owner();

        template <class T> void stopOnResolvedCancel(Promise<T>& cause);
        template <class T> void stopOnResolvedCancel(Promise<T>&& cause);

        template <class T> void stopOnResolved(Future<T>& cause);
        template <class T> void stopOnResolvedValue(Future<T>& cause);
        template <class T> void stopOnResolvedException(Future<T>& cause);
        template <class T> void stopOnResolvedCancel(Future<T>& cause);

        template <class T> void stopOnResolved(Future<T>&& cause);
        template <class T> void stopOnResolvedValue(Future<T>&& cause);
        template <class T> void stopOnResolvedException(Future<T>&& cause);
        template <class T> void stopOnResolvedCancel(Future<T>&& cause);

    private:
        sbs::Owner* sbsOwner();
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Face API_DCI_CMT current();







    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolvedCancel(Promise<T>& cause)
    {
        sbs::Owner* sbsOwner = this->sbsOwner();
        if(sbsOwner)
        {
            cause.canceled() += sbsOwner * [copy=*this]() mutable
            {
                copy.stop(false);
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolvedCancel(Promise<T>&& cause)
    {
        return stopOnResolvedCancel(cause);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolved(Future<T>& cause)
    {
        sbs::Owner* sbsOwner = this->sbsOwner();
        if(sbsOwner)
        {
            cause.then() += sbsOwner * [copy=*this](Future<T>& cause) mutable
            {
                if(cause.resolved())
                {
                    copy.stop(false);
                }
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolvedValue(Future<T>& cause)
    {
        sbs::Owner* sbsOwner = this->sbsOwner();
        if(sbsOwner)
        {
            cause.then() += sbsOwner * [copy=*this](Future<T>& cause) mutable
            {
                if(cause.resolvedValue())
                {
                    copy.stop(false);
                }
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolvedException(Future<T>& cause)
    {
        sbs::Owner* sbsOwner = this->sbsOwner();
        if(sbsOwner)
        {
            cause.then() += sbsOwner * [copy=*this](Future<T>& cause) mutable
            {
                if(cause.resolvedException())
                {
                    copy.stop(false);
                }
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolvedCancel(Future<T>& cause)
    {
        sbs::Owner* sbsOwner = this->sbsOwner();
        if(sbsOwner)
        {
            cause.then() += sbsOwner * [copy=*this](Future<T>& cause) mutable
            {
                if(cause.resolvedCancel())
                {
                    copy.stop(false);
                }
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolved(Future<T>&& cause)
    {
        return stopOnResolved(cause);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolvedValue(Future<T>&& cause)
    {
        return stopOnResolvedValue(cause);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolvedException(Future<T>&& cause)
    {
        return stopOnResolvedException(cause);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> void Face::stopOnResolvedCancel(Future<T>&& cause)
    {
        return stopOnResolvedCancel(cause);
    }
}
