// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <utility>
#include <new>
#include <exception>
#include <concepts>

namespace dci::utils
{
    template<class Fn>
    class AtScopeExit
    {
        static_assert(!std::is_reference_v<Fn>);

        static constexpr bool noexceptMoveCtor = noexcept(new(nullptr) Fn{std::declval<Fn>()});
        static constexpr bool noexceptCopyCtor = noexcept(new(nullptr) Fn{std::declval<const Fn&>()});
        static constexpr bool noexceptCall = noexcept(std::declval<Fn&>()());
        static constexpr bool noexceptDtor = noexcept(std::declval<Fn&>().~Fn());

    public:
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        // std::terminate if construction failed
        AtScopeExit(Fn&& fn) noexcept(noexceptMoveCtor) requires(std::constructible_from<Fn, Fn&&>)
            : _active{}
        {
            tripleCall([&]() noexcept(noexceptMoveCtor) {
                new(_fnSpace) Fn{std::move(fn)};
                _active = true;
            },
            [&]() noexcept {},
            [&]() noexcept {
                std::terminate();
            });
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        // std::terminate if construction failed
        AtScopeExit(AtScopeExit&& other) noexcept(noexceptMoveCtor && noexceptDtor) requires(std::constructible_from<Fn, Fn&&>)
            : _active{}
        {
            if(!other._active)
                return;

            tripleCall([&]() noexcept(noexceptMoveCtor)
            {
                new(_fnSpace) Fn{*other.pfn()};
                _active = true;
            },
            [&]() noexcept(noexceptDtor)
            {
                other.release();
            },
            [&]() noexcept
            {
                std::terminate();
            });
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        // std::terminate if construction failed
        AtScopeExit& operator=(AtScopeExit&& other) noexcept(noexceptMoveCtor && noexceptDtor) requires(std::constructible_from<Fn, Fn&&>)
        {
            if(this == &other)
                return *this;

            release();

            if(!other._active)
                return *this;

            tripleCall([&]() noexcept(noexceptMoveCtor)
            {
                new(_fnSpace) Fn{*other.pfn()};
                _active = true;
            },
            [&]() noexcept(noexceptDtor)
            {
                other.release();
            },
            [&]() noexcept
            {
                std::terminate();
            });

            return *this;
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        // functor called if construction failed
        AtScopeExit(const Fn& fn) noexcept(noexceptCopyCtor) requires(std::constructible_from<Fn, const Fn&>)
            : _active{}
        {
            tripleCall([&]() noexcept(noexceptCopyCtor)
            {
                new(_fnSpace) Fn{fn};
                _active = true;
            },
            [&]() noexcept {},
            [&]() noexcept(noexceptCall)
            {
                fn();
            });
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        // functor called if construction failed
        AtScopeExit(const AtScopeExit& other) noexcept(noexceptCopyCtor) requires(std::constructible_from<Fn, const Fn&>)
            : _active{}
        {
            if(!other._active)
                return;

            tripleCall([&]() noexcept(noexceptCopyCtor)
            {
                new(_fnSpace) Fn{const_cast<const Fn&>(*other.pfn())};
                _active = true;
            },
            [&]() noexcept {},
            [&]() noexcept(noexceptCall && noexceptDtor)
            {
                other.execute();
            });
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        AtScopeExit& operator=(const AtScopeExit& other) noexcept(noexceptCopyCtor) requires(std::constructible_from<Fn, const Fn&>)
        {
            if(this == &other)
                return *this;

            release();

            if(!other._active)
                return *this;

            tripleCall([&]() noexcept(noexceptCopyCtor)
            {
                new(_fnSpace) Fn{const_cast<const Fn&>(*other.pfn())};
                _active = true;
            },
            [&]() noexcept {},
            [&]() noexcept(noexceptCall && noexceptDtor)
            {
                other.execute();
            });

            return *this;
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        void release() noexcept(noexceptDtor)
        {
            if(!_active)
                return;

            _active = false;
            pfn()->~Fn();
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        void execute() noexcept(noexceptCall && noexceptDtor)
        {
            if(!_active)
                return;

            tripleCall([&]() noexcept(noexceptCall)
            {
                static_cast<Fn&>(*pfn())();
            },
            [&]() noexcept(noexceptDtor)
            {
                _active = false;
                pfn()->~Fn();
            },
            [&]() noexcept(noexceptDtor)
            {
                _active = false;
                pfn()->~Fn();
            });
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        ~AtScopeExit() noexcept(noexceptCall && noexceptDtor)
        {
            execute();
        }

    private:
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <class F, class Success, class Fail>
        static void tripleCall(F f, Success success, Fail fail) noexcept(noexcept(f()) && noexcept(success()))
        {
            if constexpr(noexcept(f()))
            {
                f();
                success();
            }
            else
            {
                struct FailDoer
                {
                    Fail& _fail;
                    bool _active{true};
                    ~FailDoer() noexcept(noexcept(_fail())) { if(_active) _fail(); }
                } failDoer{fail};

                f();
                failDoer._active = false;
                success();
            }
        }

    private:
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        Fn* pfn() noexcept
        {
            return std::launder(reinterpret_cast<Fn*>(_fnSpace));
        }

    private:
        alignas(Fn) char _fnSpace[sizeof(Fn)];
        bool _active;
    };
}
