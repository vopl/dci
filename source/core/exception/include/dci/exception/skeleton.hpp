// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../eid.hpp"
#include "api.hpp"
#include "buildInstance.hpp"
#include <dci/utils/tname.hpp>
#include <utility>
#include <iostream>

namespace dci::stiac
{
    template <class> class ExceptionSerializerRegistrator;
}

namespace dci::exception
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace details
    {
        template <class, class=void>
        struct StiacRegIfDefined
        {
            static int registrateUtilizer()
            {
                return 0;
            }
        };

        template <class MDE>
        struct StiacRegIfDefined<MDE, std::void_t<decltype(stiac::ExceptionSerializerRegistrator<MDE>::_registrateUtilizer)>>
        {
            static int registrateUtilizer()
            {
                return stiac::ExceptionSerializerRegistrator<MDE>::_registrateUtilizer;
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    int API_DCI_EXCEPTION registrate(
            const Eid& eid,
            const std::type_info& ti,
            std::exception_ptr (* factory)(const std::exception_ptr&));

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MDE, class Base>
    class Skeleton
        : public Base
    {
    public:
        using Base::Base;
        using Base::operator=;

    public:
        const std::string_view name() const override
        {
            (void)_registrateUtilizer;
            return std::string_view{ utils::tname<MDE>.data(), utils::tname<MDE>.size()-1 };
        }

        const Eid& eid() const override// uuidgen | sed -r 's/(..)-?/0x\1,/g' | sed -e 's/^/static constexpr Eid _eid {/' -e 's/,$/}/'
        {
            return MDE::_eid;
        }

    private:
        static volatile const int _registrateUtilizer;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MDE, class Base>
    volatile const int Skeleton<MDE, Base>::_registrateUtilizer = registrate(MDE::_eid, typeid(MDE), [](const std::exception_ptr& cause)
    {
        return buildInstance<MDE>(cause);
    }) + details::StiacRegIfDefined<MDE>::registrateUtilizer();
}
