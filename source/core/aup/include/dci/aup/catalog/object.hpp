// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../oid.hpp"
#include <dci/primitives.hpp>

#include <dci/bytes.hpp>

namespace dci::aup::catalog
{
    struct Object;
    using ObjectPtr = std::unique_ptr<Object>;

    template <class To, class From>
    To* objectPtrCast(From* from)
    {
        return dynamic_cast<To*>(from);
    }

    template <class To, class From>
    const To* objectPtrCast(const From* from)
    {
        return dynamic_cast<const To*>(from);
    }

    template <class To, class From>
    std::unique_ptr<To> objectPtrCast(std::unique_ptr<From>&& from)
    {
        To* to = dynamic_cast<To*>(from.get());
        if(to)
        {
            from.release();
            return std::unique_ptr<To>{to};
        }

        return std::unique_ptr<To>{};
    }

    struct Object
    {
        enum class Type
        {
            null        = 0,

            file        = 1,
            unit        = 2,
            release     = 3,
        };

        Set<Oid>   _dependencies;

    public:
        virtual ~Object() = default;
        virtual Type type() const = 0;
    };
}
