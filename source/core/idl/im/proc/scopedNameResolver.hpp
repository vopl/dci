/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "../errorInfo.hpp"
#include "../ast.hpp"
#include <dci/utils/dbg.hpp>
#include <vector>
#include <numeric>
#include <span>

namespace dci::idl::im::proc
{
    using namespace ast;

    class ScopedNameResolver
            : public boost::static_visitor<bool>
    {
        std::vector<ErrorInfo>& _errors;
        SScope* _rootScope{nullptr};
        SScope* _currentScope{nullptr};
        int _resolvedCount{0};
        bool _reportErrors{false};

    public:
        ScopedNameResolver(std::vector<ErrorInfo>& errors)
            : _errors(errors)
        {
        }

    public:
        bool exec(const Scope& s)
        {
            bool res = false;
            do
            {
                dbgAssert(!_rootScope);
                _rootScope = s.get();
                _currentScope = _rootScope;
                _resolvedCount = 0;
                _reportErrors = false;


                res = exec(s->decls);

                _currentScope = nullptr;
                _rootScope = nullptr;
            }
            while(!res && _resolvedCount);

            if(!res)
            {
                dbgAssert(!_rootScope);
                _rootScope = s.get();
                _currentScope = _rootScope;
                _resolvedCount = 0;
                _reportErrors = true;


                res = exec(s->decls);

                _currentScope = nullptr;
                _rootScope = nullptr;

                dbgAssert(!res);
            }

            return res;
        }

    private:
        template <class V>
        bool exec(std::vector<V>& vs)
        {
            return std::accumulate(
                vs.begin(),
                vs.end(),
                true,
                [&](bool state, const V& v)->bool
                {
                    return boost::apply_visitor(*this, v) && state;
                }
            );
        }

        bool exec(std::vector<EnumField>&)
        {
            return true;
        }

        bool exec(std::vector<FlagsField>&)
        {
            return true;
        }

        bool exec(std::vector<ExceptionField>& vs)
        {
            return std::accumulate(
                vs.begin(),
                vs.end(),
                true,
                [&](bool state, const ExceptionField& v)->bool {
                    return resolve(v->owner->owner, v->type) && state;
                }
            );
        }

        bool exec(std::vector<StructField>& vs)
        {
            return std::accumulate(
                vs.begin(),
                vs.end(),
                true,
                [&](bool state, const StructField& v)->bool {
                    return resolve(v->owner->owner, v->type) && state;
                }
            );
        }

        bool exec(std::vector<Method>& vs)
        {
            return std::accumulate(
                vs.begin(),
                vs.end(),
                true,
                [&](bool state, const Method& v)->bool {
                    state &= exec(v->query);
                    state &= resolve(v->owner->owner, v->reply);
                    return state;
                }
            );
        }

        bool exec(std::vector<MethodParam>& vs)
        {
            return std::accumulate(
                vs.begin(),
                vs.end(),
                true,
                [&](bool state, const MethodParam& v)->bool {
                    return resolve(v->owner->owner->owner, v->type) && state;
                }
            );
        }

    private:
        bool resolveInScope(const SScope* scope, SScopedName& scopedName, std::span<std::string> names, bool siblingsExpanded = false)
        {
            if(!siblingsExpanded)
            {
                for(const SScope* sibling : scope->siblings)
                {
                    if(resolveInScope(sibling, scopedName, names, true))
                    {
                        return true;
                    }
                }

                return false;
            }

            dbgAssert(!names.empty());
            if(names.empty())
                return false;

            const std::string& head = names.front();
            std::span<std::string> tail = names.subspan(1);

            auto iter = scope->name2Decl.find(head);
            if(scope->name2Decl.end() == iter)
                return false;

            return boost::apply_visitor([&](const auto& decl)
            {
                using Decl = std::decay_t<decltype(decl)>;
                if constexpr(std::is_same_v<Scope, Decl>)
                {
                    return resolveInScope(decl.get(), scopedName, tail);
                }
                else
                {
                    if constexpr(std::is_same_v<Interface, Decl>)
                    {
                        if(1 == tail.size() && "Opposite" == tail.front())
                        {
                            if(!scopedName.asScopedEntry)
                            {
                                 _resolvedCount++;
                                scopedName.asDecl = decl->opposite;
                                scopedName.asScopedEntry = decl->opposite;
                            }
                            return true;
                        }
                    }

                    if(!tail.empty())
                    {
                        return false;
                    }

                    if(!scopedName.asScopedEntry)
                    {
                        _resolvedCount++;
                        scopedName.asDecl = decl.get();
                        scopedName.asScopedEntry = decl.get();
                    }
                    return true;
                }
            }, iter->second);
        }

        bool resolve(const SScope* scope, SScopedName& scopedName)
        {
            if(scopedName.values.empty() && scopedName.asScopedEntry)
            {
                //already resolved
                return true;
            }
            dbgAssert(!scopedName.values.empty());

            std::vector<std::string> names;
            for(const Name& name : scopedName.values)
                names.emplace_back(name->value);

            if(scopedName.root)
            {
                if(resolveInScope(_rootScope, scopedName, names))
                {
                    return true;
                }

                if(_reportErrors)
                {
                    _errors.emplace_back(ErrorInfo {
                                          "unable to resolve type name `"+scopedName.toString()+"'",
                                          scopedName.pos});
                }

                return false;
            }

            while(scope)
            {
                if(resolveInScope(scope, scopedName, names))
                {
                    return true;
                }
                scope = scope->owner;
            }

            if(_reportErrors)
            {
                _errors.emplace_back(ErrorInfo {
                                      "unable to resolve type name `"+scopedName.toString()+"'",
                                      scopedName.pos});
            }

            return false;
        }

        bool resolve(const SScope* scope, TypeUse& typeUse)
        {
            ScopedName* sn = boost::get<ScopedName>(&typeUse);
            if(!sn)
            {
                return boost::apply_visitor(*this, typeUse);
            }
            dbgAssert(*sn);

            return resolve(scope, *sn->get());
        }

    public:
        bool operator()(const auto& v)
        {
            bool res = true;

            SScope* outerScope = nullptr;

            res &= beginResolveScope(v.get(), outerScope);
            res &= resolveBases(v.get());
            res &= resolveFields(v.get());

            res &= resolveAlias(v.get());

            res &= resolveElementType(v.get());
            res &= resolveValueType(v.get());
            res &= resolveKeyType(v.get());

            res &= resolveTupleTypes(v.get());
            res &= resolveVariantTypes(v.get());

            endResolveScope(v.get(), outerScope);

            return res;
        }

        bool operator()(const Interface& v)
        {
            bool res = true;

            SScope* outerScope = nullptr;

            res &= beginResolveScope(v.get(), outerScope);
            res &= resolveBases(v.get());
            res &= resolveFields(v.get());
            res &= resolveMethods(v.get());

            endResolveScope(v.get(), outerScope);

            return res;
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        bool beginResolveScope(SScope* v, SScope *&outerScope)
        {
            outerScope = _currentScope;
            _currentScope = v;
            bool res = exec(v->decls);

            return res;
        }

        bool beginResolveScope(auto, SScope *&)
        {
            return true;
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        void endResolveScope(SScope*, SScope* outerScope)
        {
            _currentScope = outerScope;
        }

        void endResolveScope(auto, SScope*)
        {
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        bool resolveBases(...)
        {
            return true;
        }

        template <class T>
        bool resolveBases(T* v, decltype(T::base)* = nullptr)
        {
            if(!v->base)
            {
                return true;
            }

            return resolveBasesImpl(v, std::vector{v->base});
        }

        template <class T>
        bool resolveBases(T* v, decltype(T::bases)* = nullptr)
        {
            return resolveBasesImpl(v, v->bases);
        }

        template <class T>
        bool resolveBasesImpl(T* v, const auto& bases)
        {
            bool res = true;

            for(auto& base : bases)
            {
                if(base->instance)
                {
                    //already resolved
                    continue;
                }

                SScopedName* sn = base->scopedName.get();

                bool localRes = resolve(v->owner, *sn);
                if(!localRes)
                {
                    res = false;
                    continue;
                }

                while(boost::get<SAlias*>(&sn->asDecl))
                {
                    SAlias* alias = boost::get<SAlias*>(sn->asDecl);

                    if(boost::get<ScopedName>(&alias->type))
                    {
                        sn = boost::get<ScopedName>(alias->type).get();
                    }
                    else
                    {
                        break;
                    }
                }

                if((!boost::get<T*>(&sn->asDecl)))
                {
                    if(_reportErrors)
                    {
                        _errors.emplace_back(ErrorInfo {
                                              "resolved type name is incompatible `"+sn->toString()+"'",
                                              sn->pos});
                    }
                    res = false;
                    continue;
                }

                if(!base->instance)
                {
                    _resolvedCount++;
                    base->instance = boost::get<std::remove_reference_t<decltype(base->instance)>>(sn->asDecl);
                }
            }

            return res;
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        bool resolveFields(...)
        {
            return true;
        }

        template <class T, class=decltype(T::fields)>
        bool resolveFields(T* v)
        {
            return exec(v->fields);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        bool resolveMethods(...)
        {
            return true;
        }

        template <class T, class=decltype(T::methods)>
        bool resolveMethods(T* v)
        {
            return exec(v->methods);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        bool resolveAlias(...)
        {
            return true;
        }

        bool resolveAlias(SAlias* v)
        {
            return resolve(v->owner, v->type);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        bool resolveElementType(...)
        {
            return true;
        }

        template <class T, class=decltype(T::elementType)>
        bool resolveElementType(T* v)
        {
            return resolve(_currentScope, v->elementType);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        bool resolveValueType(...)
        {
            return true;
        }

        template <class T, class=decltype(T::valueType)>
        bool resolveValueType(T* v)
        {
            return resolve(_currentScope, v->valueType);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        bool resolveKeyType(...)
        {
            return true;
        }

        template <class T, class=decltype(T::keyType)>
        bool resolveKeyType(T* v)
        {
            return resolve(_currentScope, v->keyType);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        bool resolveTupleTypes(...)
        {
            return true;
        }

        bool resolveTupleTypes(STuple* v)
        {
            bool res = true;

            for(TypeUse& tu: v->elementTypes)
            {
                res &= resolve(_currentScope, tu);
            }

            return res;
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        bool resolveVariantTypes(...)
        {
            return true;
        }

        bool resolveVariantTypes(SVariant* v)
        {
            bool res = true;

            for(TypeUse& tu: v->elementTypes)
            {
                res &= resolve(_currentScope, tu);
            }

            return res;
        }
    };

}
