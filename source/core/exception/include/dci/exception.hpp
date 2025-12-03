// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "exception/api.hpp"
#include "exception/skeleton.hpp"
#include "exception/buildInstance.hpp"
#include "exception/toString.hpp"
#include "eid.hpp"
#include <exception>
#include <string>

namespace dci
{
    class API_DCI_EXCEPTION Exception
        : public std::exception
    {
    public:
        Exception() = default;
        Exception(std::string_view what);
        Exception(const std::string& what);
        Exception(std::string&& what);

        template <auto size>
        Exception(const char(&what)[size])
            : Exception{std::string_view{what, size-1}}
        {
        }

        virtual const std::string_view name() const;
        virtual const Eid& eid() const;// uuidgen | sed -r 's/(..)-?/0x\1,/g' | sed -e 's/^/static constexpr Eid _eid {/' -e 's/,$/}/'

    public:
        const char* what() const noexcept override;
        const std::string& whatBuffer() const noexcept;
        std::string& whatBuffer() noexcept;

    public:
        // uuidgen | sed -r 's/(..)-?/0x\1,/g' | sed -e 's/^/static constexpr Eid _eid {/' -e 's/,$/}/'
        static constexpr Eid _eid {0xfa,0x4a,0x4e,0xf8,0x0d,0x07,0x46,0xf5,0xae,0x92,0x14,0x57,0x7e,0x86,0x1d,0xef};

    protected:
        std::string _what;
    };
}
