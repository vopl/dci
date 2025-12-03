// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::qml
{
    class Handler
        : public QObject
        , public QQmlPropertyValueSource
        , public QQmlParserStatus
    {
        Q_OBJECT
        Q_INTERFACES(QQmlPropertyValueSource)
        Q_INTERFACES(QQmlParserStatus)

        Q_PROPERTY(QVariant target      READ getTarget      WRITE   setTarget   NOTIFY targetChanged)
        Q_PROPERTY(bool     enabled     READ getEnabled     WRITE   setEnabled  NOTIFY enabledChanged)
        Q_PROPERTY(bool     charged     READ getCharged                         NOTIFY chargedChanged)
        Q_PROPERTY(bool     involved    READ getInvolved                        NOTIFY involvedChanged)

    public:
        Handler(QObject* parent=nullptr);
        ~Handler() override;

    private:// QQmlPropertyValueSource
        void setTarget(const QQmlProperty &prop) override;

    private:// QQmlParserStatus
        void classBegin() override;
        void componentComplete() override;

    private:
        QVariant getTarget() const;
        void setTarget(const QVariant& v);
        bool getEnabled() const;
        void setEnabled(bool v);
        bool getCharged() const;
        bool getInvolved() const;

    signals:
        void targetChanged();
        void enabledChanged();
        void chargedChanged();
        void involvedChanged();

    private slots:
        void update();

    private:
        Q_DISABLE_COPY(Handler)

        QQmlProperty    _targetProperty;
        QVariant        _target{};
        bool            _enabled{true};
        bool            _charged{false};
        bool            _involved{false};
        dci::sbs::Owner _connectionsOwner;

        struct Reactor
        {
            QMetaMethod _meta;
            QJSValue    _js;
        };

        QList<Reactor> _reactors;
    };
}
