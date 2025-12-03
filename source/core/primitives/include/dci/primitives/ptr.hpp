// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <memory>

namespace dci::primitives
{
    template <class T>
    struct Ptr : std::shared_ptr<T>
    {
        using Base = std::shared_ptr<T>;

        using Base::Base;
        using Base::operator=;

        Ptr() = default;
        Ptr(const Ptr&) = default;
        Ptr(Ptr&&) = default;

        Ptr& operator=(const Ptr&) = default;
        Ptr& operator=(Ptr&&) = default;

              Base&  std()       & {return *this;}
        const Base&  std() const & {return *this;}
              Base&& std()       &&{return std::move(*this);}
        const Base&& std() const &&{return std::move(*this);}

        auto operator<=>(const Ptr& rhs) const = default;
    };
}
