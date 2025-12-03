// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "../errorInfo.hpp"
#include "../ast.hpp"
#include <algorithm>

namespace dci::idl::im::proc
{
    using namespace ast;

    class OwnerIndexer
        : public boost::static_visitor<>
    {
        SScope      *_scope     {nullptr};
        SStruct     *_struct    {nullptr};
        SInterface  *_interface {nullptr};
        SMethod     *_method    {nullptr};
        SEnum       *_enum      {nullptr};
        SFlags      *_flags     {nullptr};
        SException  *_exception {nullptr};

        template <class T>
        class CurrentSetter
        {
            T *&_storage;
            T* _originalValue;

        public:
            CurrentSetter(T *&storage, T* value)
                : _storage(storage)
                , _originalValue(storage)
            {
                _storage = value;
            }

            ~CurrentSetter()
            {
                _storage = _originalValue;
            }

        };

    public:
        OwnerIndexer()
        {
        }

        void exec(const Scope& s)
        {
            CurrentSetter<SScope> css(_scope, s.get());
            exec(s->decls);
        }

    private:
        template <class V>
        void exec(std::vector<V>& vs)
        {
            std::for_each(
                vs.begin(),
                vs.end(),
                [&](const V& v)->void {boost::apply_visitor(*this, v);}
            );
        }

        void exec(std::vector<StructField>& vs)
        {
            std::for_each(
                vs.begin(),
                vs.end(),
                [&](const StructField& v)->void {
                    v->owner = _struct;
                }
            );
        }

        void exec(std::vector<EnumField>& vs)
        {
            std::for_each(
                vs.begin(),
                vs.end(),
                [&](const EnumField& v)->void {
                    v->owner = _enum;
                }
            );
        }

        void exec(std::vector<FlagsField>& vs)
        {
            std::for_each(
                vs.begin(),
                vs.end(),
                [&](const FlagsField& v)->void {
                    v->owner = _flags;
                }
            );
        }

        void exec(std::vector<ExceptionField>& vs)
        {
            std::for_each(
                vs.begin(),
                vs.end(),
                [&](const ExceptionField& v)->void {
                    v->owner = _exception;
                }
            );
        }

        void exec(std::vector<Method>& vs)
        {
            std::for_each(
                vs.begin(),
                vs.end(),
                [&](const Method& v)->void {
                    v->owner = _interface;

                    CurrentSetter<SMethod> csm(_method, v.get());
                    exec(v->query);
                }
            );
        }

        void exec(std::vector<MethodParam>& vs)
        {
            std::for_each(
                vs.begin(),
                vs.end(),
                [&](const MethodParam& v)->void {
                    v->owner = _method;
                }
            );
        }

    public:
        void operator()(const Alias& v)
        {
            v->owner = _scope;
            _scope->aliases.emplace_back(v.get());
            _scope->name2Decl.emplace(v->name->value, v);
        }

        void operator()(const Struct& v)
        {
            v->owner = _scope;
            _scope->structs.emplace_back(v.get());
            _scope->name2Decl.emplace(v->name->value, v);

            CurrentSetter csst{_struct, v.get()};
            exec(v->fields);
        }

        void operator()(const Enum& v)
        {
            v->owner = _scope;
            _scope->enums.emplace_back(v.get());
            _scope->name2Decl.emplace(v->name->value, v);

            CurrentSetter cse{_enum, v.get()};
            exec(v->fields);
        }

        void operator()(const Flags& v)
        {
            v->owner = _scope;
            _scope->flagses.emplace_back(v.get());
            _scope->name2Decl.emplace(v->name->value, v);

            CurrentSetter cse{_flags, v.get()};
            exec(v->fields);
        }

        void operator()(const Exception& v)
        {
            v->owner = _scope;
            _scope->exceptions.emplace_back(v.get());
            _scope->name2Decl.emplace(v->name->value, v);

            CurrentSetter cse{_exception, v.get()};
            exec(v->fields);
        }

        void operator()(const Interface& v)
        {
            v->owner = _scope;
            _scope->interfaces.emplace_back(v.get());
            _scope->name2Decl.emplace(v->name->value, v);

            CurrentSetter csi{_interface, v.get()};
            exec(v->methods);
        }

        void operator()(const Scope& v)
        {
            v->owner = _scope;
            _scope->scopes.emplace_back(v.get());
            _scope->name2Decl.emplace(v->name->value, v);

            CurrentSetter css{_scope, v.get()};
            exec(v->decls);
        }
    };
}
