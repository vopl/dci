/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "model.hpp"

namespace dci::qml::ep
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        const int g_role = Qt::UserRole+1;

        const QHash<int,QByteArray> g_roleNames = []
        {
            QHash<int,QByteArray> res;
            res[Qt::UserRole+1] = "epi";
            return res;
        }();

        QVector<int> g_roles = QVector<int>{} << g_role;
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class ModelPrivate
        : public QAbstractItemModelPrivate
    {
        Q_DECLARE_PUBLIC(Model)

    public:
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        ModelPrivate(const QObjectList& list)
            : QAbstractItemModelPrivate{}
            , _list{list}
        {
        }

    public:
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        void updateObjectList(const QObjectList& list)
        {
            if(_list == list)
            {
                return;
            }

            for(int row{0}; row < list.size(); ++row)
            {
                dbgAssert(_list.size() >= row);

                QObject* newO = list[row];

                if(_list.size() == row)
                {
                    //new at end
                    insert(row, newO);
                    continue;
                }

                int newPosInOld = _list.indexOf(newO, row);

                if(newPosInOld == row)
                {
                    //actual
                    continue;
                }

                if(newPosInOld >= 0)
                {
                    //known far
                    move(row, newPosInOld);
                    continue;
                }

                //unknown
                update(row, newO);
            }

            int extra = _list.size() - list.size();
            if(extra > 0)
            {
                remove(list.size(), extra);
            }

            dbgAssert(_list == list);
        }

    private:
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        void insert(int row, QObject* v)
        {
            dbgAssert(row <= _list.size());

            q_func()->beginInsertRows(QModelIndex{}, row, row);
            _list.insert(row, v);
            q_func()->endInsertRows();
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        void remove(int row, int amount)
        {
            dbgAssert(row < _list.size());
            dbgAssert(amount > 0);
            dbgAssert(row+amount <= _list.size());

            q_func()->beginRemoveRows(QModelIndex{}, row, row+amount-1);
            auto iter = _list.begin()+row;
            _list.erase(iter, iter+amount);
            q_func()->endRemoveRows();
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        void move(int srcRow, int dstRow)
        {
            dbgAssert(srcRow < _list.size());
            dbgAssert(dstRow < _list.size());

            if(srcRow < dstRow)
            {
                std::swap(srcRow, dstRow);
            }

            bool b = q_func()->beginMoveRows(QModelIndex{}, srcRow, srcRow, QModelIndex{}, dstRow);
            dbgAssert(b);
            (void)b;

            QObject* o = _list.takeAt(srcRow);
            _list.insert(dstRow, o);
            q_func()->endMoveRows();
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        void update(int row, QObject* v)
        {
            dbgAssert(row < _list.size());

            if(_list[row] != v)
            {
                _list[row] = v;
                QModelIndex idx = createIndex(row, 0);
                emit q_func()->dataChanged(idx, idx, g_roles);
            }
        }

    private:
        QObjectList _list;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Model::Model(const QObjectList& list, QObject* parent)
        : QAbstractItemModel{*new ModelPrivate{list}, parent}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Model::~Model()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QModelIndex Model::index(int row, int column, const QModelIndex& parent) const
    {
        if(parent.isValid())
        {
            return QModelIndex{};
        }

        return createIndex(row, column);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QModelIndex Model::parent(const QModelIndex& child) const
    {
        (void)child;
        return QModelIndex{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QModelIndex Model::sibling(int row, int column, const QModelIndex& idx) const
    {
        (void)row;
        (void)column;
        (void)idx;
        return QModelIndex{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    int Model::rowCount(const QModelIndex& parent) const
    {
        if(parent.isValid())
        {
            return 0;
        }

        return d_func()->_list.size();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    int Model::columnCount(const QModelIndex& parent) const
    {
        if(parent.isValid())
        {
            return 0;
        }

        return 1;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Model::hasChildren(const QModelIndex& parent) const
    {
        (void)parent;
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QVariant Model::data(const QModelIndex& index, int role) const
    {
        if(!index.isValid() || role != g_role)
        {
            return QVariant{};
        }

        if(index.column() != 0 || index.row() >= d_func()->_list.size())
        {
            return QVariant{};
        }

        return QVariant::fromValue(d_func()->_list[index.row()]);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Model::setData(const QModelIndex& index, const QVariant& value, int role)
    {
        (void)index;
        (void)value;
        (void)role;
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const
    {
        (void)section;
        (void)orientation;
        (void)role;
        return QVariant{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Model::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
    {
        (void)section;
        (void)orientation;
        (void)value;
        (void)role;
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QMap<int, QVariant> Model::itemData(const QModelIndex& index) const
    {
        (void)index;
        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Model::setItemData(const QModelIndex& index, const QMap<int, QVariant>& roles)
    {
        (void)index;
        (void)roles;
        return false;
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Model::clearItemData(const QModelIndex& index)
    {
        (void)index;
        return false;
    }
#endif

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QStringList Model::mimeTypes() const
    {
        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QMimeData* Model::mimeData(const QModelIndexList& indexes) const
    {
        (void)indexes;
        return nullptr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Model::canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const
    {
        (void)data;
        (void)action;
        (void)row;
        (void)column;
        (void)parent;
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Model::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
    {
        (void)data;
        (void)action;
        (void)row;
        (void)column;
        (void)parent;
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Qt::DropActions Model::supportedDropActions() const
    {
        return Qt::DropActions{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Qt::DropActions Model::supportedDragActions() const
    {
        return Qt::DropActions{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Model::insertRows(int row, int count, const QModelIndex& parent)
    {
        (void)row;
        (void)count;
        (void)parent;
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Model::insertColumns(int column, int count, const QModelIndex& parent)
    {
        (void)column;
        (void)count;
        (void)parent;
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Model::removeRows(int row, int count, const QModelIndex& parent)
    {
        (void)row;
        (void)count;
        (void)parent;
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Model::removeColumns(int column, int count, const QModelIndex& parent)
    {
        (void)column;
        (void)count;
        (void)parent;
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Model::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
    {
        (void)sourceParent;
        (void)sourceRow;
        (void)count;
        (void)destinationParent;
        (void)destinationChild;
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Model::moveColumns(const QModelIndex& sourceParent, int sourceColumn, int count, const QModelIndex& destinationParent, int destinationChild)
    {
        (void)sourceParent;
        (void)sourceColumn;
        (void)count;
        (void)destinationParent;
        (void)destinationChild;
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Model::fetchMore(const QModelIndex& parent)
    {
        (void)parent;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Model::canFetchMore(const QModelIndex& parent) const
    {
        (void)parent;
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Qt::ItemFlags Model::flags(const QModelIndex& index) const
    {
        (void)index;
        return Qt::ItemFlags{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Model::sort(int column, Qt::SortOrder order)
    {
        (void)column;
        (void)order;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QModelIndex Model::buddy(const QModelIndex& index) const
    {
        (void)index;
        return QModelIndex{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QModelIndexList Model::match(const QModelIndex& start, int role, const QVariant& value, int hits, Qt::MatchFlags flags) const
    {
        (void)start;
        (void)role;
        (void)value;
        (void)hits;
        (void)flags;
        return QModelIndexList{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QSize Model::span(const QModelIndex& index) const
    {
        (void)index;
        return QSize{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QHash<int,QByteArray> Model::roleNames() const
    {
        return g_roleNames;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Model::submit()
    {
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Model::revert()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Model::updateObjectList(const QObjectList& list)
    {
        d_func()->updateObjectList(list);
    }
}
