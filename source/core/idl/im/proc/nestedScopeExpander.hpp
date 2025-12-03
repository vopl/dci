// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "../ast.hpp"

namespace dci::idl::im::proc
{
    using namespace ast;

    class NestedScopeExpander
        : public boost::static_visitor<void>
    {
    public:
        void exec(const Scope& s)
        {
            operator()(s);
        }

    public:
        void operator()(const Scope& v)
        {
            if(0 == v->nestedNames.size())
            {
                //nothing
            }
            else if(1 == v->nestedNames.size())
            {
                v->name = std::move(v->nestedNames[0]);
                v->nestedNames.clear();
            }
            else
            {
                v->name = std::move(v->nestedNames[0]);
                v->nestedNames.erase(v->nestedNames.begin());

                Scope nested(new SScope);
                nested->nestedNames = std::move(v->nestedNames);
                nested->decls = std::move(v->decls);
                v->decls.clear();
                v->decls.push_back(nested);
            }

            for(const Decl& decl : v->decls)
            {
                boost::apply_visitor(*this, decl);
            }
        }

        template <class TOther>
        void operator()(const TOther&)
        {
        }
    };
}
