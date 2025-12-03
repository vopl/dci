// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "storage.hpp"

namespace dci::module::ppn::service::dht
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::put(const api::Key& key, const api::Value& value)
    {
        std::chrono::steady_clock::time_point moment = std::chrono::steady_clock::now();

        auto& idx = _container.get<ByKeyValue>();
        auto iter = idx.lower_bound(std::tie(key, value));
        if(idx.end() != iter && iter->_value == value)
        {
            idx.modify(iter, [&](ValueHolder& vh)
            {
                vh._moment = moment;
            });
            return;
        }

        idx.emplace(key, value, moment);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::get(const api::Key& key, uint32 limit, Set<api::Value>& value)
    {
        auto& idx = _container.get<ByKey>();
        auto range = idx.equal_range(key);

        while(range.first != range.second && value.size() < limit)
        {
            value.insert(range.first->_value);
            ++range.first;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::dropOld()
    {
        std::chrono::steady_clock::time_point bound = std::chrono::steady_clock::now()-std::chrono::hours{12};
        auto& idx = _container.get<ByMoment>();
        idx.erase(idx.begin(), idx.upper_bound(bound));
    }
}
