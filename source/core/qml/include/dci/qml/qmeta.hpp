// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "qmeta/def.hpp"
#include "qmeta/proxyProvider.hpp"
#include "qmeta/object.hpp"


/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
QT_BEGIN_NAMESPACE
    template<class T>
    requires (dci::qml::qmeta::Def<T>::_declared)
    struct QtPrivate::QMetaTypeTypeFlags<T>
    {
        enum
        {
            Flags = QMetaType::NeedsConstruction |
                    QMetaType::NeedsDestruction |
                    QMetaType::IsGadget
        };
    };
QT_END_NAMESPACE

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
QT_BEGIN_NAMESPACE
    template<typename T>
    requires (dci::qml::qmeta::Def<T>::_declared)
    class QtPrivate::QMetaTypeForType<T>
    {
        using Proxy = typename dci::qml::qmeta::ProxyProvider<T>::Result;

    public:
        static constexpr unsigned Flags = QMetaTypeTypeFlags<T>::Flags;

        static constexpr QMetaTypeInterface::DefaultCtrFn getDefaultCtr()
        {
            if constexpr (std::is_default_constructible_v<Proxy>)
            {
                return [](const QMetaTypeInterface*, void* addr) { new (addr) T{}; };
            }
            else
            {
                return {};
            }
        }

        static constexpr QMetaTypeInterface::CopyCtrFn getCopyCtr()
        {
            if constexpr (std::is_copy_constructible_v<Proxy>)
            {
                return [](const QMetaTypeInterface*, void* addr, const void *other)
                {
                    new (addr) Proxy{*static_cast<const Proxy*>(other)};
                };
            }
            else
            {
                return {};
            }
        }

        static constexpr QMetaTypeInterface::MoveCtrFn getMoveCtr()
        {
            if constexpr (std::is_move_constructible_v<Proxy>)
            {
                return [](const QMetaTypeInterface* , void* addr, void* other)
                {
                    new (addr) Proxy{std::move(*static_cast<Proxy*>(other))};
                };
            }
            else
            {
                return {};
            }
        }

        static constexpr QMetaTypeInterface::DtorFn getDtor()
        {
            return [](const QMetaTypeInterface *, void *addr)
            {
                static_cast<Proxy*>(addr)->~Proxy();
            };
        }

        static constexpr QMetaTypeInterface::LegacyRegisterOp getLegacyRegister()
        {
            return nullptr;
            //return []() { QMetaTypeId2<T>::qt_metatype_id(); };
            //return []() { qRegisterNormalizedMetaType<T>(getName()); };
        }

        static constexpr const char *getName()
        {
            //return dci::qml::qmeta::Def<T>::_name._buf;

            //лучше взять из Stringdata, чтобы не дублировать строку в бинарь
            constexpr uint idx = dci::qml::qmeta::Stringdata<T>::template _indexFor<dci::qml::qmeta::Def<T>::_name>;
            return dci::qml::qmeta::Stringdata<T>::getStr(idx);
        }
    };
QT_END_NAMESPACE

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
QT_BEGIN_NAMESPACE
    template<class T>
    requires (dci::qml::qmeta::Def<T>::_declared)
    struct QtPrivate::MetaObjectForType<T, void>
    {
        static const QMetaObject* value()
        {
            return dci::qml::qmeta::Object<T>::qt();
        }

        static const QMetaObject* metaObjectFunction(const QMetaTypeInterface*)
        {
            return value();
        }
    };
QT_END_NAMESPACE

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
QT_BEGIN_NAMESPACE
    template <typename T>
    requires (dci::qml::qmeta::Def<T>::_declared)
    struct QMetaTypeId<T>
    {
        enum { Defined = 1 };
        static int qt_metatype_id()
        {
            return QMetaType::fromType<T>().id();
        }
    };
QT_END_NAMESPACE
