// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/exception.hpp>
#include <dci/exception/skeleton.hpp>

namespace dci
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Exception::Exception(std::string_view what)
        : _what(what)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Exception::Exception(const std::string& what)
        : _what(what)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Exception::Exception(std::string&& what)
        : _what(std::move(what))
    {
    }

    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        const int g_registrateUtilizer = exception::registrate(Exception::_eid, typeid(Exception), [](const std::exception_ptr& cause)
        {
            return exception::buildInstance<Exception>(cause);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const std::string_view Exception::name() const
    {
        (void)g_registrateUtilizer;
        return std::string_view{ utils::tname<Exception>.data(), utils::tname<Exception>.size()-1 };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Eid& Exception::eid() const// uuidgen | sed -r 's/(..)-?/0x\1,/g' | sed -e 's/^/static constexpr Eid _eid {/' -e 's/,$/}/'
    {
        return _eid;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const char* Exception::what() const noexcept
    {
        return _what.c_str();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const std::string& Exception::whatBuffer() const noexcept
    {
        return _what;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string& Exception::whatBuffer() noexcept
    {
        return _what;
    }
}
