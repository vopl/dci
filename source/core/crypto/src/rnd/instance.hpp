/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/crypto/rnd.hpp>
#include "../impl/chaCha.hpp"
#include "../impl/hmac.hpp"
#include "entropy/source.hpp"
#include <vector>

namespace dci::crypto::rnd
{
    class Instance
    {
    public:
        Instance();
        ~Instance();

        bool generate(void* buf, std::size_t len);

    public:
        void addEntropy(void* e, std::size_t len, std::size_t entropyAmount);

    private:
        void obtainMoreEntropy();

        template <class S> void tryUseEntropySource();

    private:
        std::vector<entropy::SourcePtr> _entropySources;
        std::size_t                     _nextEntropySource {0};
        impl::Hmac                      _hmac;
        impl::ChaCha                    _chacha;

        static constexpr std::size_t    _minEntropy = 64;
        static constexpr std::size_t    _maxEntropy = 1024;
        static constexpr std::size_t    _outPerEntropy = 1024;

        std::size_t                     _entropyAvailable {0};
        std::size_t                     _outEmitted {0};
    };
}
