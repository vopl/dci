// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include <cstddef>
#include "api.hpp"
#include <dci/himpl.hpp>
#include <dci/mm/implMetaInfo.hpp>

namespace dci::mm
{
    ////////////////////////////////////////////////////////////////
    class API_DCI_MM Stack
        : public dci::himpl::FaceLayout<Stack, impl::Stack>
    {
        using Base = dci::himpl::FaceLayout<Stack, impl::Stack>;

    public:
        Stack();
        Stack(const Stack& from) = delete;
        Stack(Stack&& from);
        ~Stack();

        Stack& operator=(const Stack& from) = delete;
        Stack& operator=(Stack&& from);

        void initialize();
        bool initialized() const;

    public:
        bool growsDown() const;
        bool hasGuard() const;

        char* begin() const;
        char* end() const;
        std::size_t size() const;

        void compact();
    };
}
