// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
