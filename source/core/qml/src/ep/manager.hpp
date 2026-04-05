/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "pch.hpp"
#include <dci/qml/app.hpp>
#include "selection.hpp"

namespace dci::qml::ep
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Manager
    {
    private:
        Manager(const Manager&) = delete;
        void operator=(const Manager&) = delete;

    protected:
        Manager();
        ~Manager();

    public:
        void registrate(Extension* e, const QString& tag, int priority = 0);
        void deregistrate(Extension* e);
        void deregistrate(Extension* e, const QString& tag);

    public:
        void startup(App* app);
        void shutdown(App* app);

    public:
        SelectionPtr select(const QString& tag, std::size_t max = ~std::size_t{});

    public:
        App* app();
        QList<Extension *> getExtensions(const QString& tag, std::size_t max);
        void selectionUninvolved(SelectionPtr s);

    private:
        App* _app{};

    private:
        struct ExtState
        {
            Extension * _e;
            QString     _tag;
            int         _priority;
        };

        using ExtKeyExtractor0 = boost::multi_index::member<ExtState,Extension *,&ExtState::_e>;

        using ExtKeyExtractor1 = boost::multi_index::member<ExtState,QString,&ExtState::_tag>;

        using ExtKeyExtractor2 = boost::multi_index::composite_key
        <
            ExtState,
            ExtKeyExtractor0,
            ExtKeyExtractor1
        >;

        using ExtKeyExtractor3 = boost::multi_index::composite_key
        <
            ExtState,
            ExtKeyExtractor1,
            boost::multi_index::member<ExtState,int,&ExtState::_priority>
        >;

        using Extensions = boost::multi_index::multi_index_container
        <
            ExtState,
            boost::multi_index::indexed_by
            <
                boost::multi_index::ordered_non_unique <ExtKeyExtractor0>,
                boost::multi_index::ordered_non_unique <ExtKeyExtractor1>,
                boost::multi_index::ordered_non_unique <ExtKeyExtractor2>,
                boost::multi_index::ordered_non_unique <ExtKeyExtractor3,std::greater<ExtKeyExtractor3::result_type>>
            >
        >;

        Extensions _extensions;

    private:
        struct SelState
        {
            SelectionPtr _s;

            const QString& tag() const
            {
                return _s->tag();
            }

            std::size_t max() const
            {
                return _s->max();
            }
        };

        using SelKeyExtractor0 = boost::multi_index::member<SelState,SelectionPtr,&SelState::_s>;

        using SelKeyExtractor1 = boost::multi_index::const_mem_fun<SelState,const QString&,&SelState::tag>;

        using SelKeyExtractor2 = boost::multi_index::composite_key
        <
            SelState,
            boost::multi_index::const_mem_fun<SelState,const QString&,&SelState::tag>,
            boost::multi_index::const_mem_fun<SelState,std::size_t,&SelState::max>
        >;

        using Selections = boost::multi_index::multi_index_container
        <
            SelState,
            boost::multi_index::indexed_by
            <
                boost::multi_index::ordered_unique     <SelKeyExtractor0>,
                boost::multi_index::ordered_non_unique <SelKeyExtractor1>,
                boost::multi_index::ordered_non_unique <SelKeyExtractor2,std::greater<SelKeyExtractor2::result_type>>
            >
        >;

        Selections _selections;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    extern Manager& g_manager;
}
