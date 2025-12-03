// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "rdseed.hpp"
#include "../../instance.hpp"
#include <cpuid.h>
#include <immintrin.h>

namespace dci::crypto::rnd::entropy::source
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool RDSEED::available()
    {
        std::uint32_t eax, ebx, ecx, edx;
        __cpuid (7, eax, ebx, ecx, edx);
        return ebx & bit_RDSEED;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const std::string_view RDSEED::name()
    {
        return std::string_view("RDSEED");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void RDSEED::flush()
    {
        std::uint64_t buf[4];
        std::size_t cnt = 0;
        std::size_t fails = 0;
        while(cnt<4)
        {
            char ok = 0;
            asm volatile ("rdseed %0; setc %b1" : "=r"(buf[cnt]), "=qm"(ok) :: "cc");

            if(ok)
            {
                cnt++;
                fails = 0;
            }
            else
            {
                fails++;
                if(fails > 10)
                {
                    break;
                }
            }
        }

        if(cnt)
        {
            _instance->addEntropy(buf, sizeof(buf[0])*cnt, sizeof(buf[0])*cnt);
        }
    }
}
