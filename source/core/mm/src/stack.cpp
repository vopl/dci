// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/mm/stack.hpp>
#include "impl/stack.hpp"

namespace dci::mm
{
    ////////////////////////////////////////////////////////////////
    Stack::Stack()
        : Base()
    {
    }

    Stack::Stack(Stack&& from)
        : Base(std::move(from.impl()))
    {
    }

    Stack::~Stack()
    {
    }

    Stack& Stack::operator=(Stack&& from)
    {
        Base::operator=(std::move(from));
        return *this;
    }

    void Stack::initialize()
    {
        return impl().initialize();
    }

    bool Stack::initialized() const
    {
        return impl().initialized();
    }

    bool Stack::growsDown() const
    {
        return impl().growsDown();
    }

    bool Stack::hasGuard() const
    {
        return impl().hasGuard();
    }

    char* Stack::begin() const
    {
        return impl().begin();
    }

    char* Stack::end() const
    {
        return impl().end();
    }

    std::size_t Stack::size() const
    {
        return impl().size();
    }

    void Stack::compact()
    {
        return impl().compact();
    }

}
