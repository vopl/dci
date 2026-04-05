/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "def.hpp"
#include "api/prop.hpp"
#include "api/meth.hpp"
#include "api/ctor.hpp"
#include "api/info.hpp"
#include "stringdata.hpp"
#include <QMetaType>

namespace dci::qml::qmeta
{
    namespace data::details
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <class Api>
        using MetaTypesFetcher = typename Api::MetaTypes;

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        using MetaTypeFix = void(*)(uint*);

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <class TypAndIndex>
        using IsNotBuiltInType = Value<!QMetaTypeId2<typename TypAndIndex::template Get<0>>::IsBuiltIn>;

        template <class Typ, std::size_t offset> constexpr MetaTypeFix metaTypeFix()
        {
            return [](uint* data)
            {
                data[offset] = QMetaType::fromType<Typ>().id();
            };
        }

        template <class TypsList, std::size_t offset> constexpr auto metaTypeFixes()
        {
            return []<class... Typs, class... Indices>(TList<Typs...>, TList<Indices...>)
            {
                return []<class... Typs2, class... Indices2>(TList<TList<Typs2, Indices2>...>)
                {
                    return std::array<MetaTypeFix, sizeof...(Typs2)>{metaTypeFix<Typs2, offset+Indices2::_v>()...};
                }(typename TList<TList<Typs, Indices>...>::template Grep<IsNotBuiltInType>{});
            }(TypsList{}, MakeSeq<TypsList::_size>{});
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <class Typ>
        constexpr uint id4BuiltInType()
        {
            if constexpr(QMetaTypeId2<Typ>::IsBuiltIn)
            {
                return QMetaTypeId2<Typ>::MetaType;
            }
            else
            {
                return QMetaType::UnknownType;//будет пофикшено в рантайме
            }
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <std::size_t dataSize, std::size_t fixesSize>
        struct Props
        {
            std::array<uint, dataSize> _data;
            std::array<MetaTypeFix, fixesSize> _fixes;
        };

        template <class T, class ApiList>
        static constexpr auto propsCollect()
        {
            auto accumuler = [&]<auto idx>(const auto& self, auto alreadyData, auto alreadyFixes, Value<idx>)
            {
                if constexpr(idx >= ApiList::_size)
                {
                    return Props<alreadyData.size(), alreadyFixes.size()>{alreadyData, alreadyFixes};
                }
                else
                {
                    using Api = typename ApiList::template Get<idx>;

                    std::array<uint, 5> data
                    {
                        // name
                        Stringdata<T>::template _indexFor<Api::_name>,

                        // type
                        id4BuiltInType<typename Api::Type>(),

                        // flags
                        // qtbase/src/corelib/kernel/qmetaobject_p.h
                        //enum PropertyFlags {
                        //    Invalid = 0x00000000,
                        //    Readable = 0x00000001,
                        //    Writable = 0x00000002,
                        //    Resettable = 0x00000004,
                        //    EnumOrFlag = 0x00000008,
                        //    Alias = 0x00000010,
                        //    // Reserved for future usage = 0x00000020,
                        //    StdCppSet = 0x00000100,
                        //    Constant = 0x00000400,
                        //    Final = 0x00000800,
                        //    Designable = 0x00001000,
                        //    Scriptable = 0x00004000,
                        //    Stored = 0x00010000,
                        //    User = 0x00100000,
                        //    Required = 0x01000000,
                        //    Bindable = 0x02000000
                        //};
                        0x00004800 |
                                (Api::_isPropR ? 0x00000001 : 0) |
                                (Api::_isPropW ? 0x00000002 : 0x00000400),

                        // notifyId
                        uint(-1),

                        // revision
                        0
                    };

                    auto nextData = concatenate(alreadyData, data);
                    auto nextFixes = concatenate(alreadyFixes, metaTypeFixes<typename Api::MetaTypes, alreadyData.size()+1>());

                    return self(self, nextData, nextFixes, Value<idx+1>{});
                }
            };

            return accumuler(accumuler, std::array<uint, 0>{}, std::array<MetaTypeFix, 0>{}, Value<0>{});
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <std::size_t bodySize, std::size_t paramsSize, std::size_t fixesSize>
        struct Meths
        {
            std::array<uint, bodySize>          _body;
            std::array<uint, paramsSize>        _params;
            std::array<MetaTypeFix, fixesSize>  _paramsFixes;
        };

        template <class T, class ApiList, bool isCtorsAdoption = false>
        static constexpr auto methsCollect(uint offset=0, uint metaTypesOffset = 0)
        {
            auto accumuler = [&]<auto idx>(const auto& self, auto alreadyBody, auto alreadyParams, auto alreadyParamFixes, Value<idx>)
            {
                if constexpr(idx >= ApiList::_size)
                {
                    return Meths<alreadyBody.size(), alreadyParams.size(), alreadyParamFixes.size()>{alreadyBody, alreadyParams, alreadyParamFixes};
                }
                else
                {
                    using Api = typename ApiList::template Get<idx>;

                    using ParamTypes = typename Api::ParamTypes;
                    using MetaTypes = typename Api::MetaTypes;

                    std::array<uint, isCtorsAdoption ? 0 : 1> returnType;
                    if constexpr(!isCtorsAdoption)
                    {
                        returnType[0] = id4BuiltInType<typename Api::Return>();
                    }

                    std::array<uint, ParamTypes::_size> paramTypes{};
                    std::array<uint, ParamTypes::_size> paramNames{};
                    [&]<auto... pidx>(VList<pidx...>)
                    {
                        ((paramTypes[pidx] = id4BuiltInType<typename ParamTypes::template Get<pidx>>()), ...);
                        ((paramNames[pidx] = Stringdata<T>::template _indexFor<Api::ParamNames::template Get<pidx>::_v>), ...);
                    }(MakeSeq<ParamTypes::_size>{});

                    std::array<uint, 6> body
                    {
                        // name
                        Stringdata<T>::template _indexFor<Api::_name>,

                        // argc
                        ParamTypes::_size,

                        // parameters
                        offset + static_cast<uint>(alreadyParams.size()),

                        // tag
                        Stringdata<T>::template _indexFor<Name{""}>,

                        // flags
                        0x02|0x00,// это из приватной части Qt: QT_NAMESPACE::AccessPublic | QT_NAMESPACE::MethodMethod,

                        // initial metatype offsets
                        metaTypesOffset
                    };

                    metaTypesOffset += MetaTypes::_size;

                    auto nextBody = concatenate(alreadyBody, body);
                    auto nextParams = concatenate(alreadyParams, returnType, paramTypes, paramNames);
                    auto nextParamFixes = concatenate(alreadyParamFixes, metaTypeFixes<MetaTypes, alreadyParams.size()>());

                    return self(self, nextBody, nextParams, nextParamFixes, Value<idx+1>{});
                }
            };

            return accumuler(accumuler, std::array<uint, 0>{}, std::array<uint, 0>{}, std::array<MetaTypeFix, 0>{}, Value<0>{});
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <std::size_t dataSize>
        struct Infos
        {
            std::array<uint, dataSize> _data;
        };

        template <class T, class ApiList>
        static constexpr auto infosCollect()
        {
            Infos<ApiList::_size*2> res;

            [&]<auto... idx>(VList<idx...>)
            {
                ((res._data[idx*2+0] = Stringdata<T>::template _indexFor<ApiList::template Get<idx>::_key>), ...);
                ((res._data[idx*2+1] = Stringdata<T>::template _indexFor<ApiList::template Get<idx>::_value>), ...);
            }(MakeSeq<ApiList::_size>{});

            return res;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    requires (Def<T>::_declared)
    struct Data
    {
        using GD = Def<T>;

        using Props = typename GD::Api::template Grep<api::IsProp>;
        using Meths = typename GD::Api::template Grep<api::IsMeth>;
        using Ctors = typename GD::Api::template Grep<api::IsCtor>;
        using Infos = typename GD::Api::template Grep<api::IsInfo>;

        using PropsMetaTypes = typename Props::template Map<data::details::MetaTypesFetcher>::template Linearize<>;
        using MethsMetaTypes = typename Meths::template Map<data::details::MetaTypesFetcher>::template Linearize<>;
        using CtorsMetaTypes = typename Ctors::template Map<data::details::MetaTypesFetcher>::template Linearize<>;

        static constexpr std::size_t _headerSize = 14;

        static constexpr uint _propsOffset          = _headerSize;
        static constexpr auto _props                = data::details::propsCollect<T, Props>();

        static constexpr uint _methsOffset          = _propsOffset + _props._data.size();
        static constexpr auto _methsBillet          = data::details::methsCollect<T, Meths>();
        static constexpr auto _meths                = data::details::methsCollect<T, Meths>(_methsOffset + _methsBillet._body.size(), PropsMetaTypes::_size);

        static constexpr uint _ctorsOffset          = _methsOffset + _meths._body.size() + _meths._params.size();
        static constexpr auto _ctorsBillet          = data::details::methsCollect<T, Ctors, true>();
        static constexpr auto _ctors                = data::details::methsCollect<T, Ctors, true>(_ctorsOffset + _ctorsBillet._body.size(), PropsMetaTypes::_size + MethsMetaTypes::_size);

        static constexpr uint _infosOffset          = _ctorsOffset + _ctors._body.size() + _ctors._params.size();
        static constexpr auto _infos                = data::details::infosCollect<T, Infos>();

        static constexpr std::array<uint, _headerSize> _header =
        {
            // content:
            9,                                              // revision
            Stringdata<T>::template _indexFor<GD::_name>,   // classname
            Infos::_size, _infosOffset,                     // classinfo
            Meths::_size, _methsOffset,                     // methods
            Props::_size, _propsOffset,                     // properties
            0,            0,                                // enums/sets
            Ctors::_size, _ctorsOffset,                     // constructors
            4,                                              // flags    //MetaObjectFlag::PropertyAccessInStaticMetaCall
            0,                                              // signalCount
        };

        static const uint* qt()
        {
            static const auto result = []
            {
                constexpr auto billet = concatenate(
                    _header,
                    _props._data,
                    _meths._body, _meths._params,
                    _ctors._body, _ctors._params,
                    _infos._data,
                    std::array<uint, 1>{0});

                auto result = billet;

                for(auto fix : _props._fixes)
                {
                    fix(result.data() + _propsOffset);
                }

                for(auto fix : _meths._paramsFixes)
                {
                    fix(result.data() + _methsOffset + _meths._body.size());
                }

                for(auto fix : _ctors._paramsFixes)
                {
                    fix(result.data() + _ctorsOffset + _ctors._body.size());
                }

                return result;
            }();

            return result.data();
        }

        static constexpr const QtPrivate::QMetaTypeInterface* const* qtMetaTypes()
        {
            return []<class... E1, class... E2, class... E3>(TList<E1...>, TList<E2...>, TList<E3...>)
            {
                if constexpr(sizeof...(E1) + sizeof...(E2) + sizeof...(E3))
                    return qt_metaTypeArray<E1..., E2..., E3...>;
                else
                    return nullptr;
            }(PropsMetaTypes{}, MethsMetaTypes{}, CtorsMetaTypes{});
        }
    };
}
