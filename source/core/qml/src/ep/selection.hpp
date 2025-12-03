// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "model.hpp"

namespace dci::qml::ep
{
    class Manager;
    struct Extension;
    class Selection
        : public QObject
        , public std::enable_shared_from_this<Selection>
    {
        Q_OBJECT

    public:
        Selection(Manager* m, const QString& tag, std::size_t max);
        ~Selection() override;

        const QString& tag() const;
        std::size_t max() const;

        void detached();

    public:
        void update();

    public:
        QAbstractItemModel* asModel();
        QObjectList asArray();
        QObject* asObject(std::size_t index = 0);

    private:
        Manager* _m;
        QString _tag;
        std::size_t _max;

    private:
        QList<Extension *> _extensions;

    private:
        QObjectList     _objects;
        QPointer<Model> _model;
    };

    using SelectionPtr = std::shared_ptr<Selection>;
}
