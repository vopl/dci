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

namespace dci::qml::ep
{
    class ModelPrivate;

    class Model
        : public QAbstractItemModel
    {
        Q_OBJECT

    public:
        explicit Model(const QObjectList& list, QObject* parent = nullptr);
        ~Model() override;

        QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex& child) const override;

        QModelIndex sibling(int row, int column, const QModelIndex& idx) const override;
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        int columnCount(const QModelIndex& parent = QModelIndex()) const override;
        bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
        bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::EditRole) override;

        QMap<int, QVariant> itemData(const QModelIndex& index) const override;
        bool setItemData(const QModelIndex& index, const QMap<int, QVariant>& roles) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        bool clearItemData(const QModelIndex& index) override;
#endif

        QStringList mimeTypes() const override;
        QMimeData* mimeData(const QModelIndexList& indexes) const override;
        bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const override;
        bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;
        Qt::DropActions supportedDropActions() const override;

        Qt::DropActions supportedDragActions() const override;

        bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
        bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;
        bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
        bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;
        bool moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild) override;
        bool moveColumns(const QModelIndex& sourceParent, int sourceColumn, int count, const QModelIndex& destinationParent, int destinationChild) override;

        void fetchMore(const QModelIndex& parent) override;
        bool canFetchMore(const QModelIndex& parent) const override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;
        void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
        QModelIndex buddy(const QModelIndex& index) const override;
        QModelIndexList match(const QModelIndex& start, int role, const QVariant& value, int hits = 1, Qt::MatchFlags flags = Qt::MatchFlags(Qt::MatchStartsWith|Qt::MatchWrap)) const override;
        QSize span(const QModelIndex& index) const override;

        QHash<int,QByteArray> roleNames() const override;

        bool submit() override;
        void revert() override;

    public:
        void updateObjectList(const QObjectList& list);

    private:
        Q_DISABLE_COPY(Model)
        Q_DECLARE_PRIVATE(Model)
    };
}
