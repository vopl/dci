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
#include <vector>
#include <map>
#include <numeric>

namespace dci::idl::im::proc
{
    using namespace ast;

    class NameChecker
        : public boost::static_visitor<bool>
    {
        std::vector<ErrorInfo>& _errors;

        std::map<std::string, Name> _entities;
        std::map<std::string, Name> _scopes;

    public:
        NameChecker(std::vector<ErrorInfo>& errors)
            : _errors(errors)
        {}

    public:
        bool exec(const Scope& s)
        {
            return exec(s->decls);
        }

    private:
        bool exec(const std::vector<Decl>& decls)
        {
            return std::accumulate(
                decls.begin(),
                decls.end(),
                true,
                [&](bool v, const Decl& d)->bool{return boost::apply_visitor(*this, d) && v;}
            );
        }

    public:
        template <class T>
        bool operator()(const T& v)
        {
            bool res = true;
            res &= checkName(v.get());
            res &= checkMethods(v.get());
            res &= checkFields(v.get());
            res &= checkQuery(v.get());
            res &= checkChildren(v.get());
            return res;
        }

    private:
//        bool checkName(...)
//        {
//            return true;
//        }

        bool checkName(const SScope* v)
        {
            const Name& cur = v->name;

            _scopes.insert(std::make_pair(v->prepareFullScopedName()->toString(), cur));

            auto fres = _entities.find(v->prepareFullScopedName()->toString());
            if(_entities.end() != fres)
            {
                _errors.emplace_back(ErrorInfo {
                                      "duplicate name `"+cur->value+"'",
                                      cur->pos});

                const Name& prev = fres->second;

                _errors.emplace_back(ErrorInfo {
                                      "previous declaration of `"+prev->value+"'",
                                      prev->pos});

                return false;
            }

            return true;
        }

        bool checkName(const SScopeEntry* v)
        {
            const Name& cur = v->name;
            Name prev;

            auto ires = _entities.insert(std::make_pair(v->prepareFullScopedName()->toString(), cur));
            if(!ires.second)
            {
                prev = ires.first->second;
            }

            auto fres = _scopes.find(v->prepareFullScopedName()->toString());
            if(_scopes.end() != fres)
            {
                prev = fres->second;
            }

            if(prev)
            {
                _errors.emplace_back(ErrorInfo {
                                      "duplicate name `"+cur->value+"'",
                                      cur->pos});

                _errors.emplace_back(ErrorInfo {
                                      "previous declaration of `"+prev->value+"'",
                                      prev->pos});

                return false;
            }

            return true;
        }

    private:

        template <class T>
        static auto getFieldsOrQuery(const T* v)
        {
            return v->fields;
        }

        static auto getFieldsOrQuery(const SMethod* v)
        {
            return v->query;
        }

        static auto getFieldsOrQuery(const SInterface* v)
        {
            return v->methods;
        }

        template <class T>
        bool checkFieldsOrQuery(const T* v)
        {
            std::map<std::string, Name> names;

            bool res = true;
            for(const auto& f : getFieldsOrQuery(v))
            {
                const Name& cur = f->name;

                if(cur)
                {
                    auto ires = names.insert(std::make_pair(cur->value, cur));
                    if(!ires.second)
                    {
                        _errors.emplace_back(ErrorInfo {
                                              "duplicate name `"+cur->value+"'",
                                              cur->pos});

                        const Name& prev = ires.first->second;

                        _errors.emplace_back(ErrorInfo {
                                              "previous declaration of `"+prev->value+"'",
                                              prev->pos});
                        res = false;
                    }
                }
            }

            return res;
        }

    private:
        bool checkFields(...)
        {
            return true;
        }

        template <class T, class=decltype(T::fields)>
        bool checkFields(const T* v)
        {
            return checkFieldsOrQuery(v);
        }

    private:
        bool checkQuery(...)
        {
            return true;
        }

        bool checkQuery(const SMethod* v)
        {
            return checkFieldsOrQuery(v);
        }

    private:
        bool checkMethods(...)
        {
            return true;
        }

        bool checkMethods(const SInterface* v)
        {
            return checkFieldsOrQuery(v);
        }

    private:
        bool checkChildren(...)
        {
            return true;
        }

        bool checkChildren(const SScope* v)
        {
            return exec(v->decls);
        }
    };

}
