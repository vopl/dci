// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/sbs/wire.hpp>
#include "id.hpp"
#include "methodId.hpp"
#include <memory>

namespace dci::stiac::link
{
    class Hub4Link;
    class Source;

    class Base
    {
        Base(const Base&) = delete;
        void operator=(const Base&) = delete;

    protected:
        Base();
        virtual ~Base() = default;

    public:
        virtual void initialize(Hub4Link* hub, Id id);
        virtual void deinitialize();
        virtual void input(Source& source) = 0;
        virtual void destroy() = 0;

    public:
        struct Deleter
        {
            void operator()(Base* ptr) const;
        };

    protected:
        template <class R, class... Args>
        R call2Bin(MethodId methodId, Args&&... args);

        template <class R, class... Args>
        void bin2Call(Source& source, auto&& sink);

        template <class R, class... Args>
        void bin2Call(Source& source, sbs::Wire<R, Args...>& sink);

    protected:
        Hub4Link*   _hub;
        Id          _id;
    };

    using BasePtr = std::unique_ptr<Base, Base::Deleter>;

}
