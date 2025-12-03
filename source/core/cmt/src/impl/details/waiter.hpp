// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/cmt/details/wwLink.hpp>
#include <dci/primitives.hpp>
#include "../ctx/fiber.hpp"

namespace dci::cmt::impl::details
{
    using namespace dci::cmt::details;

    class Waitable;

    class Waiter final
    {
        using ExprEvaluator = bool(*)(void* eeData);

    public:
        Waiter(WWLink* links, std::size_t amount);
        Waiter(WWLink* links, std::size_t amount, void(*cb)(void* cbData), void* cbData);
        ~Waiter();

        void any(std::size_t* acquiredIndex);
        void all();
        void expr(ExprEvaluator ee, void* eeData, std::byte* bitsInBytes);

        void reset();

    public:
        ctx::Fiber* fiber();
        template <bool positive=true> bool readyOffer(WWLink* link);
        void waitableDead(WWLink* link);

    private:
        struct ModeSync
        {
            ctx::Fiber* _fiber{};
        };

        struct ModeAsync
        {
            void(*_cb)(void* cbData){};
            void* _cbData{};
        };

    private:
        struct StateNull
        {
        };

        struct StateAll
        {
        };

        struct StateAny
        {
            std::size_t* _acquiredIndex{};
        };

        struct StateExpr
        {
            ExprEvaluator   _evaluator{};
            void*           _evaluatorData{};
            std::byte*      _bitsInBytes{};

            bool getBit(std::size_t index) const;
            void setBit(std::size_t index);
            void resetBit(std::size_t index);
        };

    private:
        template <class State> void exec(auto&&... args);

        template <bool positive=true> bool ready(StateNull& state, WWLink* offeredFrom = {});
        template <bool positive=true> bool ready(StateAll& state, WWLink* offeredFrom = {});
        template <bool positive=true> bool ready(StateAny& state, WWLink* offeredFrom = {});
        template <bool positive=true> bool ready(StateExpr& state, WWLink* offeredFrom = {});

        void commit(StateNull& state, WWLink* offeredFrom = {});
        void commit(StateAll& state, WWLink* offeredFrom = {});
        void commit(StateAny& state, WWLink* offeredFrom = {});
        void commit(StateExpr& state, WWLink* offeredFrom = {});

        void beginAcquire();
        void endAcquire();

    private:
        WWLink *    _links{};
        std::size_t _linksAmount{};

        Variant<ModeSync, ModeAsync> _mode;
        Variant<StateNull, StateAll, StateAny, StateExpr> _state{};
    };

}
