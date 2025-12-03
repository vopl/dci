// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "instance.hpp"
#include <dci/crypto/sha2_256.hpp>

#include "entropy/source/systime.hpp"
#include "entropy/source/rdrand.hpp"
#include "entropy/source/rdseed.hpp"
#ifdef _WIN32
#   include "entropy/source/std.hpp"
#else
#   include "entropy/source/devRandom.hpp"
#   include "entropy/source/devUrandom.hpp"
#endif

#include <dci/utils/dbg.hpp>
#include <cstring>

namespace dci::crypto::rnd
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class S> void Instance::tryUseEntropySource()
    {
        if(S::available())
        {
            //std::cout<<"dci-crypto use entropy source: "<<S::name()<<std::endl;
            _entropySources.emplace_back(std::make_unique<S>(this));
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Instance::Instance()
        : _hmac{Sha2_256::alloc()}
        , _chacha{20}
    {
        using namespace entropy::source;

        tryUseEntropySource<SysTime>();
        tryUseEntropySource<RDSEED>();
        tryUseEntropySource<RDRAND>();
#ifdef _WIN32
        tryUseEntropySource<Std>();
#else
        tryUseEntropySource<DevRandom>();
        tryUseEntropySource<DevUrandom>();
#endif
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Instance::~Instance()
    {
        _entropySources.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Instance::generate(void* buf, std::size_t len)
    {
        uint8_t* buf1 = static_cast<uint8_t*>(buf);

        bool res = true;

        while(len)
        {
            if(_entropyAvailable < _minEntropy)
            {
                obtainMoreEntropy();

                if(_entropyAvailable < _minEntropy)
                {
                    res = false;
                }
            }

            std::size_t toProcess;
            if(_entropyAvailable)
            {
                toProcess = std::min(len, _outPerEntropy);
            }
            else
            {
                toProcess = len;
            }

            _chacha.cipher(nullptr, buf1, toProcess);

            buf1 += toProcess;
            len -= toProcess;
            _outEmitted += toProcess;

            if(_outEmitted >= _outPerEntropy)
            {
                std::size_t entropyEmitted = _outEmitted/_outPerEntropy;
                if(entropyEmitted >= _entropyAvailable)
                {
                    _entropyAvailable = 0;
                    _outEmitted = 0;
                }
                else
                {
                    _entropyAvailable -= entropyEmitted;
                    _outEmitted = _outEmitted % _outPerEntropy;
                }
            }
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::addEntropy(void* e, std::size_t len, std::size_t entropyAmount)
    {
        _hmac.add(e, len);

        std::array<uint8_t, 32> mac;
        dbgAssert(mac.size() == _hmac.digestSize());

        _hmac.finish(mac.data());
        _chacha.setKey(mac.data(), mac.size());

        _chacha.cipher(nullptr, mac.data(), mac.size());
        _hmac.setKey(mac.data(), mac.size());

        _entropyAvailable += entropyAmount;

        if(_entropyAvailable > _maxEntropy)
        {
            _entropyAvailable = _maxEntropy;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::obtainMoreEntropy()
    {
        if(_entropySources.empty())
        {
            return;
        }

        std::size_t ineffectiveAttemts = 0;

        while(_entropyAvailable < _minEntropy)
        {
            std::size_t prevEntropyAvailable = _entropyAvailable;

            _entropySources[_nextEntropySource % _entropySources.size()]->flush();
            _nextEntropySource++;

            if(prevEntropyAvailable == _entropyAvailable)
            {
                ineffectiveAttemts++;
            }
            else
            {
                ineffectiveAttemts = 0;
            }

            if(ineffectiveAttemts > _entropySources.size())
            {
                break;
            }
        }
    }
}
