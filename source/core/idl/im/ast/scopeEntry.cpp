// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "scopeEntry.hpp"
#include "scope.hpp"
#include "scopedName.hpp"
#include <dci/utils/dbg.hpp>

namespace dci::idl::im::ast
{
    ScopedName SScopeEntry::prepareFullScopedName() const
    {
        ScopedName res(new SScopedName);
        if(name)
        {
            res->pos = name->pos;
        }
        res->root = true;

        SScopeEntry* se = const_cast<SScopeEntry *>(this);
        while(se && se->owner)
        {
            dbgAssert(se->name);
            res->values.insert(res->values.begin(), se->name);
            se = se->owner;
        }

        res->asScopedEntry = const_cast<SScopeEntry *>(this);

        return res;
    }

}
