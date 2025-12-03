// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "scopedName.hpp"
#include "scope.hpp"
#include "interface.hpp"
#include <dci/utils/dbg.hpp>
#include <numeric>

namespace dci::idl::im::ast
{
    std::string SScopedName::toString(const std::string& delim) const
    {
        return std::accumulate(
                    values.begin(),
                    values.end(),
                    std::string(),
                    [&](const std::string& state, const Name& v){return state.empty() ? v->value : state+delim+v->value;});
    }

    ScopedName SScopedName::toFullScopedName() const
    {
        ScopedName res (new SScopedName);
        res->pos = this->pos;
        res->root = true;

        SScopeEntry* se = this->asScopedEntry;
        while(se && se->owner)
        {
            dbgAssert(se->name);
            res->values.insert(res->values.begin(), se->name);
            se = se->owner;
        }

        res->asDecl = this->asDecl;
        res->asScopedEntry = this->asScopedEntry;

        return res;
    }
}
