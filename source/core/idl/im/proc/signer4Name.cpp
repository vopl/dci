// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "signer4Name.hpp"
#include "../signBuilder.hpp"
#include <dci/utils/dbg.hpp>

namespace dci::idl::im::proc
{
    using namespace ast;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Signer4Name::Signer4Name()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Signer4Name::exec(Scope& s)
    {
        visit(s.get());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class ScopeEntry> void Signer4Name::process(ScopeEntry* e)
    {
        SignBuilder sb;

        sb.add(nodeTag<ScopeEntry>);

        if constexpr(std::is_same_v<ScopeEntry, SInterface>)
        {
            sb.add("side");
            sb.add(e->isPrimary ? "primary" : "opposite");
        }

        if(e->name)
        {
            sb.add("name");
            sb.add(e->name->value4Abi.value_or(e->name->value));
        }

        if(e->owner)
        {
            dbgAssert(e->owner->sign4Name != Sign{});
            sb.add("owner");
            sb.add(e->owner->sign4Name);
        }

        e->sign4Name = sb.finish();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Signer4Name::visit(SScope* v)
    {
        process(v);

        for(auto e : v->aliases      ) process(e);
        for(auto e : v->structs      ) process(e);
        for(auto e : v->enums        ) process(e);
        for(auto e : v->flagses      ) process(e);
        for(auto e : v->exceptions   ) process(e);
        for(auto e : v->interfaces   ) process(e);
        for(auto e : v->interfaces   ) process(e->opposite);

        for(auto e : v->scopes       ) visit(e);
    }
}
