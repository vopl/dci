// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/utils/dns.hpp>
#include <dci/utils/dbg.hpp>
#include <dci/primitives.hpp>
#include <dci/utils/uri.hpp>

namespace dci::utils::dns
{
    namespace
    {
        class Utf8Traversor
        {
            std::string_view    _input;
            std::size_t         _processed{};

        public:
            Utf8Traversor(std::string_view input)
                : _input{input}
            {}

            uint32 advance()
            {
                if(_processed >= _input.size())
                    return {};

                std::size_t len;
                {
                    unsigned char uc = _input[_processed];
                    if(uc < 0x80)
                    {
                        _processed += 1;
                        return uc;
                    }
                    else if(uc < 0xE0)
                        len = 2;
                    else if(uc < 0xF0)
                        len = 3;
                    else
                        len = 4;
                }

                std::size_t end = _processed + len;
                if(end > _input.size())
                    end = _input.size();

                uint32 codepoint = _input[_processed] & (0x7F >> len);
                _processed += 1;
                do
                {
                    codepoint <<= 6;
                    codepoint |= _input[_processed] & 0x3F;
                    _processed += 1;
                }
                while(_processed < end);

                return codepoint;
            }

            void reset()
            {
                _processed = {};
            }
        };
    }

    namespace
    {
        constexpr uint64 base = 36;
        constexpr uint64 tmin = 1;
        constexpr uint64 tmax = 26;
        constexpr uint64 skew = 38;
        constexpr uint64 damp = 700;

        bool isUpper(char c)
        {
            return c >= 'A' && c <= 'Z';
        }

        char toLower(char c)
        {
            if(isUpper(c))
                return c - 'A' + 'a';
            return c;
        }

        char encodeDigit(uint32 d)
        {
            return d + 22 + 75 * (d < 26);
        }

        uint64 adapt(uint64 delta, uint64 numpoints, bool firsttime)
        {
            delta = firsttime ? delta / damp : delta >> 1;
            delta += delta / numpoints;

            uint64 k{};
            for(; delta > ((base - tmin) * tmax) / 2; k += base)
                delta /= base - tmin;

            return k + (base - tmin + 1) * delta / (delta + skew);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    // to lower + idn prefix + punycode
    CanonicalizePartResult canonicalizePart(std::string_view src, std::string* dstPtr)
    {
        uint64 basicPrefix{};
        bool hasBasicUpper{};
        uint64 srcCodepoints{};

        Utf8Traversor srcTraversor{src};
        while(uint32 cp = srcTraversor.advance())
        {
            ++srcCodepoints;
            if(cp < 0x80)
            {
                hasBasicUpper |= isUpper(static_cast<char>(cp));
                ++basicPrefix;
            }
            else if(cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDBFF))
                return CanonicalizePartResult::badInput;
        }

        if(basicPrefix == srcCodepoints && !hasBasicUpper)
            return CanonicalizePartResult::unneeded;

        if(!dstPtr)
            return CanonicalizePartResult::badOutput;
        std::string& dst = *dstPtr;

        if(basicPrefix == srcCodepoints)
        {
            for(char c : src)
                dst.push_back(toLower(c));
            return CanonicalizePartResult::ok;
        }

        dst += "xn--";

        uint64 b;
        uint64 h = b = basicPrefix;
        uint64 delta = 0;
        uint64 bias = 72;
        uint64 n = 0x80;

        while(h < srcCodepoints)
        {
            uint64 m = UINT64_MAX;

            srcTraversor.reset();
            while(uint32 cp = srcTraversor.advance())
            {
                if(basicPrefix)
                {
                    if(cp < 0x80)
                    {
                        dst.push_back(toLower(static_cast<char>(cp)));
                        --basicPrefix;
                        if(!basicPrefix)
                            dst.push_back('-');
                    }
                }
                if(cp >= n && cp < m)
                    m = cp;
            }

            delta += (m - n) * (h + 1);
            n = m;

            srcTraversor.reset();
            while(uint32 cp = srcTraversor.advance())
            {
                if(cp < n)
                    ++delta;

                if(cp == n)
                {
                    uint64 q = delta;
                    for(uint64 k = base;; k += base)
                    {
                        uint64 t = k <= bias ? tmin : k >= bias + tmax ? tmax : k - bias;
                        if(q < t)
                            break;
                        dst.push_back(encodeDigit(t + (q - t) % (base - t)));
                        q = (q - t) / (base - t);
                    }

                    dst.push_back(encodeDigit(q));
                    bias = adapt(delta, h + 1, h == b);
                    delta = 0;
                    ++h;
                }
            }

            ++delta, ++n;
        }

        return CanonicalizePartResult::ok;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    CanonicalizeResult canonicalize(std::string& domain, bool dotsOnSides)
    {
        if(!domain.empty())
        {
            uri::networkNode::Host<std::string_view> networkNode;
            if(uri::parse(domain, networkNode))
            {
                if(std::get_if<uri::networkNode::Ip4<std::string_view>>(&networkNode))
                    return CanonicalizeResult::ip;
                if(std::get_if<uri::networkNode::Ip6<std::string_view>>(&networkNode))
                    return CanonicalizeResult::ip;
            }
        }

        auto iterateParts = [&](auto f)
        {
            std::size_t partBegin = 0;
            std::size_t partEnd = domain.find('.', partBegin);
            partEnd = partEnd > domain.size() ? domain.size() : partEnd;
            do
            {
                std::string_view part{domain.begin() + partBegin, domain.begin() + partEnd};
                f(part);

                partBegin = partEnd+1;
                partEnd = domain.find('.', partBegin);
                partEnd = partEnd > domain.size() ? domain.size() : partEnd;
            }
            while(partBegin < domain.size());
        };

        bool isBadInput = false;
        bool isOk = !domain.empty();
        std::size_t estimatedSize = 0;
        estimatedSize += 1;
        iterateParts([&, isFirstPart = true](std::string_view part) mutable
        {
            if(isFirstPart)
                isFirstPart = false;
            else
            {
                if(part.empty())
                {
                    isBadInput = true;
                    return;
                }
            }

            estimatedSize += 1;
            switch(canonicalizePart(part))
            {
            case CanonicalizePartResult::unneeded:
                estimatedSize += part.size();
                estimatedSize += 1;
                break;
            case CanonicalizePartResult::badOutput:
                estimatedSize += 4;
                estimatedSize += part.size()*2;
                estimatedSize += 1;
                isOk = false;
                break;
            case CanonicalizePartResult::ok:
                dbgFatal("unreacheable");
                break;
            case CanonicalizePartResult::badInput:
                isBadInput = true;
                break;
            }
        });

        if(isBadInput)
            return CanonicalizeResult::badInput;

        if(isOk)
        {
            if(dotsOnSides)
            {
                if(domain.front()=='.' && domain.back() == '.')
                    return CanonicalizeResult::unneeded;
            }
            else
            {
                if(domain.empty() || (domain.front()!='.' && domain.back() != '.'))
                    return CanonicalizeResult::unneeded;
            }
        }

        std::string modified;
        modified.reserve(estimatedSize);
        iterateParts([&](std::string_view part)
        {
            if(part.empty())
                return;

            if(dotsOnSides || !modified.empty())
                modified.push_back('.');

            switch(canonicalizePart(part, &modified))
            {
            case CanonicalizePartResult::unneeded:
                modified += part;
                break;
            case CanonicalizePartResult::badOutput:
                dbgFatal("unreacheable");
                break;
            case CanonicalizePartResult::ok:
                break;
            case CanonicalizePartResult::badInput:
                dbgFatal("unreacheable");
                break;
            }
        });
        if(dotsOnSides)
            modified.push_back('.');

        modified.swap(domain);

        return CanonicalizeResult::ok;
    }
}
