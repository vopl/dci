// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "voidStream.hpp"
#include "autoTag.hpp"
#include "destinationDetector.hpp"

#include <dci/utils/dbg.hpp>
#include <functional>
#include <utility>
#include <tuple>

namespace dci::cmt::details::transformHolder
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace call
    {
        template <class TStream>
        struct OppositeFor
        {
            using Result = typename TStream::Opposite;
        };

        template <>
        struct OppositeFor<void>
        {
            using Result = void;
        };

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        struct BadResult {};

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template<
            class F,
            class SourceStream,
            template<class...> class TDestinationStream,
            class TDestination>
        struct Impl
        {
        private:
            std::decay_t<F> _f;

            using This = Impl<F, SourceStream, TDestinationStream, TDestination>;


        private:

            template <class... ExtraArgs>
            decltype(auto) doInvoke(ExtraArgs&&... extraArgs) requires (std::is_invocable_v<F, ExtraArgs...>)
            {
                return std::invoke(std::forward<F>(_f), std::forward<ExtraArgs>(extraArgs)...);
            }

            template <class... ExtraArgs>
            BadResult doInvoke(ExtraArgs&&...) requires (!std::is_invocable_v<F, ExtraArgs...>)
            {
                return {};
            }

        public:
            using DestinationStream = TDestinationStream<TDestination>;
            using DestinationStreamOpposite = typename call::OppositeFor<DestinationStream>::Result;

            auto invoke(SourceStream* source, DestinationStream* destination)
            {
                if constexpr(!std::is_void_v<SourceStream>)
                {
                    dbgAssert(source);
                }

                if constexpr(!std::is_void_v<SourceStream>)
                {
                    dbgAssert(source);
                }

                (void)source;
                (void)destination;

                if constexpr(std::is_void_v<SourceStream>)
                {
                    if constexpr(std::is_void_v<DestinationStream>)
                    {
                        //source        void
                        //destination   void

                             if constexpr(std::is_same_v<TDestination, decltype(doInvoke())>)
                        {
                            return doInvoke();
                        }
                        else
                        {
                            return BadResult();
                        }
                    }
                    else
                    {
                        //source        void
                        //destination   nonvoid

                             if constexpr(std::is_same_v<void, decltype(doInvoke(std::move(*destination)))>)
                        {
                            return doInvoke(std::move(*destination));
                        }
                        else if constexpr(std::is_same_v<void, decltype(doInvoke(*destination))>)
                        {
                            return doInvoke(*destination);
                        }
                        else if constexpr(std::is_same_v<TDestination, decltype(doInvoke())>)
                        {
                            return doInvoke();
                        }
                        else
                        {
                            return BadResult();
                        }
                    }
                }
                else
                {
                    if constexpr(std::is_void_v<DestinationStream>)
                    {
                        //source        nonvoid
                        //destination   void

                             if constexpr(std::is_same_v<TDestination, decltype(doInvoke(std::move(*source)))>)
                        {
                            return doInvoke(std::move(*source));
                        }
                        else if constexpr(std::is_same_v<TDestination, decltype(doInvoke(*source))>)
                        {
                            return doInvoke(*source);
                        }
                        else if constexpr(std::is_same_v<TDestination, decltype(doInvoke())>)
                        {
                            return doInvoke();
                        }
                        else
                        {
                            return BadResult();
                        }

                    }
                    else
                    {
                        //source        nonvoid
                        //destination   nonvoid

                             if constexpr(std::is_same_v<void, decltype(doInvoke(std::move(*source), std::move(*destination)))>)
                        {
                            return doInvoke(std::move(*source), std::move(*destination));
                        }
                        else if constexpr(std::is_same_v<void, decltype(doInvoke(*source, std::move(*destination)))>)
                        {
                            return doInvoke(*source, std::move(*destination));
                        }
                        else if constexpr(std::is_same_v<void, decltype(doInvoke(std::move(*source), *destination))>)
                        {
                            return doInvoke(std::move(*source), *destination);
                        }
                        else if constexpr(std::is_same_v<void, decltype(doInvoke(*source, *destination))>)
                        {
                            return doInvoke(*source, *destination);
                        }

                        else if constexpr(std::is_same_v<TDestination, decltype(doInvoke(std::move(*source)))>)
                        {
                            return doInvoke(std::move(*source));
                        }
                        else if constexpr(std::is_same_v<TDestination, decltype(doInvoke(*source))>)
                        {
                            return doInvoke(*source);
                        }

                        else if constexpr(std::is_same_v<void, decltype(doInvoke(std::move(*destination)))>)
                        {
                            return doInvoke(std::move(*destination));
                        }
                        else if constexpr(std::is_same_v<void, decltype(doInvoke(*destination))>)
                        {
                            return doInvoke(*destination);
                        }

                        else if constexpr(std::is_same_v<TDestination, decltype(doInvoke())>)
                        {
                            return doInvoke();
                        }
                        else
                        {
                            return BadResult();
                        }
                    }
                }
            }

            Impl(F&& f)
                : _f(std::forward<F>(f))
            {
            }
        };

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template<
            class F,
            class SourceStream,
            template<class...> class TDestinationStream>
        struct Impl<F, SourceStream, TDestinationStream, transformHolder::AutoTag>
            : public Impl<F, SourceStream, TDestinationStream, typename DestinationDetector<F, SourceStream, TDestinationStream>::Result>
        {
            using Base = Impl<F, SourceStream, TDestinationStream, typename DestinationDetector<F, SourceStream, TDestinationStream>::Result>;

            using Base::Base;
        };


        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template<
            class F,
            class SourceStream>
        struct Impl<F, SourceStream, transformHolder::VoidStream, transformHolder::AutoTag>
            : public Impl<F, SourceStream, transformHolder::VoidStream, void>
        {
            using Base = Impl<F, SourceStream, transformHolder::VoidStream, void>;

            using Base::Base;
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<
        class F,
        class SourceStream,
        template<class...> class TDestinationStream,
        class TDestination>
    struct Call
        : public call::Impl<F, SourceStream, TDestinationStream, TDestination>
    {
        using Base = call::Impl<F, SourceStream, TDestinationStream, TDestination>;

        using Base::Base;

        using InvokeResult = std::invoke_result_t<decltype(&Base::invoke), Base, SourceStream*, typename Base::DestinationStream*>;

        static constexpr bool _valid = !std::is_same_v<call::BadResult, InvokeResult>;
    };
}
