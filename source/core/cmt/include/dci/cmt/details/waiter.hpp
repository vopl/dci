// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/cmt/api.hpp>
#include <dci/himpl.hpp>
#include <dci/cmt/implMetaInfo.hpp>
#include <dci/cmt/details/wwLink.hpp>

namespace dci::cmt::details
{
    class API_DCI_CMT Waiter final
        : public himpl::FaceLayout<Waiter, impl::details::Waiter>
    {
        Waiter(const Waiter&) = delete;
        void operator=(const Waiter&) = delete;

    public:
        using ExprEvaluator = bool(*)(void* eeData);

    public:
        Waiter(WWLink* links, std::size_t amount);
        Waiter(WWLink* links, std::size_t amount, void(*cb)(void* cbData), void* cbData);
        ~Waiter();

        void any(std::size_t* acquiredIndex);
        void all();
        void expr(ExprEvaluator ee, void* eeData, std::byte* bitsInBytes);

        void reset();
    };

}
