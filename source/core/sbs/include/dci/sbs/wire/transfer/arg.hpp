// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <type_traits>
#include <utility>
#include <cstdint>
#include <dci/utils/dbg.hpp>

namespace dci::sbs::wire::transfer
{

    template <class T, bool isSmallTrivial = (std::is_trivial_v<T> && sizeof(T)<=sizeof(void*))>
    class Arg;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    class Arg<T, true>
    {
    public:
        explicit Arg(T v);

        constexpr static const bool _preferConstAllways = true;
        bool preferConst() const;

    public:
        operator T&&() const;
        operator const T&() const requires(true);

        T&& rr() const;
        const T& cr() const requires(true);

    private:
        mutable T _v;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    class Arg<T, false>
    {
    public:
        Arg() = delete;
        Arg(const Arg&) = delete;
        Arg(Arg&&) = delete;

        void operator=(const Arg&) = delete;
        void operator=(Arg&&) = delete;

    public:
        explicit Arg(const T& v);
        explicit Arg(T&& v);

        ~Arg();

        constexpr static const bool _preferConstAllways = false;
        bool preferConst() const;

    public:
        operator T&&() const;
        operator const T&() const requires(true);

        T&& rr() const;
        const T& cr() const requires(true);

    private:
        T& copy() const;

    private:

        enum class ValueKind : std::uintptr_t
        {
            ptr,
            cptr,
            copy
        } mutable _valueKind;

        union
        {
            T * const _ptr;
            const T * const _cptr;
            alignas(T) mutable std::byte _copy[sizeof(T)];
        };
    };




    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Arg<T, false>::Arg(const T& v)
        : _valueKind(ValueKind::cptr)
        , _cptr(&v)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Arg<T, false>::Arg(T&& v)
        : _valueKind(ValueKind::ptr)
        , _ptr(&v)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Arg<T, false>::~Arg()
    {
        if(ValueKind::copy == _valueKind)
        {
            copy().~T();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    bool Arg<T, false>::preferConst() const
    {
        return ValueKind::cptr == _valueKind;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Arg<T, false>::operator T&&() const
    {
        return rr();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Arg<T, false>::operator const T&() const requires(true)
    {
        return cr();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    T&& Arg<T, false>::rr() const
    {
        switch(_valueKind)
        {
        case ValueKind::ptr:
            return std::move(*_ptr);

        case ValueKind::cptr:
            {
                const T* cptr = _cptr;
                _valueKind = ValueKind::copy;
                new (&copy()) T{*cptr};
            }
            [[fallthrough]];

        case ValueKind::copy:
            break;
        }

        dbgAssert(ValueKind::copy == _valueKind);
        return std::move(copy());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    const T& Arg<T, false>::cr() const requires(true)
    {
        switch(_valueKind)
        {
        case ValueKind::ptr:
            return *_ptr;

        case ValueKind::cptr:
            return *_cptr;

        case ValueKind::copy:
            break;
        }

        dbgAssert(ValueKind::copy == _valueKind);
        return copy();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    T& Arg<T, false>::copy() const
    {
        return *static_cast<T*>(static_cast<void*>(&_copy));
    }














    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Arg<T, true>::Arg(T v)
        : _v(v)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    bool Arg<T, true>::preferConst() const
    {
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Arg<T, true>::operator T&&() const
    {
        return rr();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    Arg<T, true>::operator const T&() const requires(true)
    {
        return cr();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    T&& Arg<T, true>::rr() const
    {
        return std::move(_v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    const T& Arg<T, true>::cr() const requires(true)
    {
        return _v;
    }

}
