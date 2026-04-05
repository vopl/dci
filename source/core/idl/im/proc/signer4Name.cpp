/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
