/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/utils/intrusiveDlist.hpp>
#include <dci/utils/dbg.hpp>
#include <QQmlApplicationEngine>
#include <QJSValue>
#include "../../../app.hpp"

namespace dci::qml::qmeta::def::container::face
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    struct Iterable;
}

namespace dci::qml::qmeta::def::container
{
    namespace iterator
    {
        enum class Mode
        {
            null,
            key,
            value,
            keyValue
        };
    }

    template <class T>
    class Iterator
        : public utils::IntrusiveDlistElement<Iterator<T>>
    {
    public:
        Iterator();
        Iterator(face::Iterable<T>* container, iterator::Mode mode = {});
        Iterator(const Iterator& from);
        ~Iterator();

        Iterator& operator=(const Iterator& from);

        bool nextImpl();
        QJSValue value();
        std::size_t key();

        QJSValue next();
        void reset();

    private:
        face::Iterable<T>* _container{};
        iterator::Mode _mode{};
    };
}

namespace dci::qml::qmeta::def::container
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Container>
    Iterator<Container>::Iterator()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Iterator<T>::Iterator(face::Iterable<T>* container, iterator::Mode mode)
        : _container{container}
        , _mode{mode}
    {
        dbgAssert(iterator::Mode::null != _mode);
        dbgAssert(_container);
        _container->addIterator(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Iterator<T>::Iterator(const Iterator& from)
        : Iterator{from._container, from._mode}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Iterator<T>::~Iterator()
    {
        if(_container)
        {
            _container->delIterator(this);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Iterator<T>& Iterator<T>::operator=(const Iterator& from)
    {
        if(this != &from)
        {
            if(_container)
            {
                _container->delIterator(this);
            }

            _mode = from._mode;
            _container = from._container;

            if(_container)
            {
                _container->addIterator(this);
            }
        }

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    bool Iterator<T>::nextImpl()
    {
        dbgFatal("not impl");
        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    QJSValue Iterator<T>::value()
    {
        dbgFatal("not impl");
        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    std::size_t Iterator<T>::key()
    {
        dbgFatal("not impl");
        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    QJSValue Iterator<T>::next()
    {
        //https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Iteration_protocols
        QJSValue res = App::instance()->qengine()->newObject();
        res.setProperty(QStringLiteral("done"), QJSValue{true});
        res.setProperty(QStringLiteral("value"), QJSValue{});

        dbgWarn("not impl");

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void Iterator<T>::reset()
    {
        if(_container)
        {
            _container->delIterator(this);
            _container = nullptr;
        }
        _mode = iterator::Mode::null;
    }
}
