// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "args.hpp"
#include "../../../eval.hpp"

namespace dci::module::ppn::node::rdb::instance::eval::function::utils
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Args::Args(const List<pql::Expression>& raw, const table::Record& record)
        : _raw(raw)
        , _record(record)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const table::Record& Args::record() const
    {
        return _record;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Args::size() const
    {
        return _raw.size();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const pql::Value& Args::operator[](std::size_t idx) const
    {
        if(idx >= size())
        {
            dbgWarn("bad index");
            return _nullStub;
        }

        while(_combined.size() <= idx)
        {
            _combined.emplace_back(combine(_raw[_combined.size()], _record).getOr(_nullStub));
        }

        return _combined[idx];
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const List<pql::Value>& Args::head(std::size_t amount) const
    {
        dbgAssert(_raw.size() >= amount);

        while(_combined.size() < amount)
        {
            _combined.emplace_back(combine(_raw[_combined.size()], _record).getOr(_nullStub));
        }

        return _combined;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const pql::Value Args::_nullStub {};
}
