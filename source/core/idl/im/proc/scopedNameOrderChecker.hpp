// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "../errorInfo.hpp"
#include "../ast.hpp"

#include <dci/utils/dbg.hpp>
#include <vector>
#include <algorithm>

namespace dci::idl::im::proc
{
    using namespace ast;

    class ScopedNameOrderChecker
        : public boost::static_visitor<>
    {
        std::vector<ErrorInfo>&     _errors;
        std::set<SScopeEntry*>      _declared;

    public:
        ScopedNameOrderChecker(std::vector<ErrorInfo>& errors)
            : _errors(errors)
        {}

    public:
        void exec(Scope& s)
        {
            dbgAssert(_errors.empty());

            _declared.clear();

            (*this)(s.get());

            _declared.clear();
        }

    public:

        void operator()(TypeUse& v)
        {
            if(boost::get<ScopedName>(&v))
            {
                (*this)(boost::get<ScopedName>(v).get());
            }
        }

        void operator()(SScopedName* v)
        {
            check(v);
            if(!_errors.empty()) return;
        }

        void operator()(SAlias* v)
        {
            declare(v);

            (*this)(v->type);
        }

        void operator()(SStruct* v)
        {
            for(StructBase& b : v->bases)
            {
                (*this)(b->scopedName.get());
            }

            declare(v);

            for(StructField& f : v->fields)
            {
                (*this)(f->type);
            }
        }

        void operator()(SEnum* v)
        {
            declare(v);
        }

        void operator()(SFlags* v)
        {
            declare(v);
        }

        void operator()(SException* v)
        {
            if(v->base)
            {
                (*this)(v->base->scopedName.get());
            }

            declare(v);

            for(ExceptionField& f : v->fields)
            {
                (*this)(f->type);
            }
        }

        void operator()(SInterface* v)
        {
            for(InterfaceBase& b : v->bases)
            {
                (*this)(b->scopedName.get());
            }

            declare(v);
            declare(v->opposite);

            for(Method& m : v->methods)
            {
                for(MethodParam& mp : m->query)
                {
                    (*this)(mp->type);
                }

                (*this)(m->reply);
            }
        }

        void operator()(SScope* v)
        {
            for(Decl& d : v->decls)
            {
                boost::apply_visitor([&](const auto& sptr)
                {
                    (*this)(sptr.get());
                }, d);
            }
        }

    private:
        void declare(SScopeEntry* se)
        {
            _declared.insert(se);
        }

        void check(SScopedName* v)
        {
            SScopeEntry* se = v->asScopedEntry;
            dbgAssert(se);

            if(_declared.end() == std::find(_declared.begin(), _declared.end(), se))
            {
                _errors.emplace_back(ErrorInfo {
                                      "using before declaration `"+v->toString()+"'",
                                      v->pos});

                _errors.emplace_back(ErrorInfo {
                                      "declaration here `"+se->name->value+"'",
                                      se->name->pos});
            }

        }
    };
}
