// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <array>
#include <algorithm>

namespace dci::utils
{
    namespace tnameDetails
    {
        template <class T>
        constexpr auto fsig()
        {
            //TODO std::source_location::function_name
            return std::to_array(__PRETTY_FUNCTION__);
        }

        template <class T>
        constexpr auto name()
        {
            constexpr auto probe = tnameDetails::fsig<void>();
            constexpr auto probeTarget = std::to_array("void");
            constexpr std::size_t prefix = static_cast<std::size_t>(std::search(probe.begin(), probe.end()-1, probeTarget.begin(), probeTarget.end()-1) - probe.begin());
            constexpr std::size_t postfix = (probe.size()-1) - prefix - (probeTarget.size()-1);

            constexpr auto victim = tnameDetails::fsig<T>();

            std::array<char, victim.size()-prefix-postfix> res{};
            std::copy_n(victim.data()+prefix, res.size()-1, res.data());

            return res;
        }
    }

    template <class T>
    static constexpr auto tname = tnameDetails::name<T>();
}
