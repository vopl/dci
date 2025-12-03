// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/logger/stream.hpp>
#include <dci/logger/timeProvider.hpp>
#include <cstdio>

namespace dci::logger
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Stream::Stream(std::string_view level, std::string_view identity)
    {
        _buf << std::boolalpha;
        _buf << std::setprecision(std::numeric_limits<double>::digits10);

        auto putOne = [&](bool needSpace, std::string_view str) -> bool
        {
            if(str.empty())
            {
                return false;
            }

            if(needSpace)
                _buf << ' ';
            _buf << str;
            return true;
        };

        bool needSpace = false;
        needSpace |= putOne(needSpace, timeProvidedAsString());
        needSpace |= putOne(needSpace, level);
        needSpace |= putOne(needSpace, identity);
        _buf << ": ";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Stream::~Stream()
    {
        _buf << '\n';
        std::string str{std::move(_buf).str()};
        std::fwrite(str.c_str(), str.size(), 1, stdout);
        std::fflush(stdout);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Stream& Stream::operator<<(const std::error_code& ec)
    {
        _buf << ec.message() << " (" << ec.category().name() << '.' << ec.value() << ')';
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Stream& Stream::operator<<(const std::error_condition& ec)
    {
        _buf << ec.message() << " (" << ec.category().name() << '.' << ec.value() << ')';
        return *this;
    }
}
