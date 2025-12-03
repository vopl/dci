// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "statsIA.hpp"

namespace dci::module::ppn::connectivity::reest
{
    const KeyIA& stat2Key(const StatIA& stat)
    {
        union U
        {
            int _stub;
            const StatIARecord _rec;

            U() : _stub{} {}
            ~U() {}
        } u{};

        const char* p1 = static_cast<const char*>(static_cast<const void*>(&u._rec.first));
        const char* p2 = static_cast<const char*>(static_cast<const void*>(&u._rec.second));
        std::size_t offset = p2-p1;

        const char* p = static_cast<const char*>(static_cast<const void*>(&stat));
        p = p - offset;

        return *static_cast<const KeyIA*>(static_cast<const void*>(p));
    }
}
