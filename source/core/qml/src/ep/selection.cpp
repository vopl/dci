// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "selection.hpp"
#include <dci/qml/app.hpp>
#include <dci/qml/ep.hpp>
#include "manager.hpp"

namespace dci::qml::ep
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Selection::Selection(Manager* m, const QString& tag, std::size_t max)
        : _m{m}
        , _tag{tag}
        , _max{max}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Selection::~Selection()
    {
        for(QObject* o : _objects)
        {
            if(o) disconnect(o);
        }

        if(_model)
        {
            _model->updateObjectList(QObjectList{});
            _model->deleteLater();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const QString& Selection::tag() const
    {
        return _tag;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Selection::max() const
    {
        return _max;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Selection::detached()
    {
        _m = nullptr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Selection::update()
    {
        QMap<Extension *, QObject *> old;
        for(int idx{0}; idx<_extensions.size(); ++idx)
        {
            old.insert(_extensions[idx], _objects[idx]);
        }

        _extensions = _m->getExtensions(_tag, _max);
        _objects.clear();

        for(int idx{0}; idx<_extensions.size(); ++idx)
        {
            Extension* e = _extensions[idx];
            QObject* o = old.value(e);

            if(!o)
            {
                o = e->activate(_m->app(), _tag);
                _m->app()->qengine()->setObjectOwnership(o, QQmlEngine::JavaScriptOwnership);
                connect(o, &QObject::destroyed, this, &Selection::update);

                QQmlComponent* qc = qobject_cast<QQmlComponent*>(o);

                if(qc)
                {
                    static const QByteArray stub = "import QtQml 2.12; QtObject{}";

                    switch(qc->status())
                    {
                    case QQmlComponent::Null:
                        LOGE("extension at \""<<_tag.toStdString()<<"\" is null");
                        qc->setData(stub, QUrl{});
                        break;
                    case QQmlComponent::Ready:
                        break;
                    case QQmlComponent::Loading:
                        LOGW("extension at \""<<_tag.toStdString()<<"\" is not ready");
                        qc->setData(stub, QUrl{});
                        break;
                    case QQmlComponent::Error:
                        LOGE("extension at \""<<_tag.toStdString()<<"\" failed: "<<qc->errorString().trimmed().toStdString());
                        qc->setData(stub, QUrl{});
                    }
                }
            }
            else
            {
                old[e] = nullptr;
            }

            _objects << o;
        }

        for(QObject* o : old)
        {
            if(o) disconnect(o);
        }

        if(_model)
        {
            _model->updateObjectList(_objects);
        }

//        if(_objects.empty())
//        {
//            _m->selectionUninvolved(this->shared_from_this());
//        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QAbstractItemModel* Selection::asModel()
    {
        if(!_model)
        {
            _model = new Model{_objects};
            _m->app()->qengine()->setObjectOwnership(_model, QQmlEngine::JavaScriptOwnership);
        }

        return _model.data();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QObjectList Selection::asArray()
    {
        return _objects;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QObject* Selection::asObject(std::size_t index)
    {
        if(static_cast<int>(index) <= _objects.size())
        {
            return _objects[static_cast<int>(index)];
        }

        return nullptr;
    }
}
