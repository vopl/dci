// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
