// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "content.hpp"

#include "config.hpp"
#ifdef HAVE_VALGRIND
#   include <valgrind.h>
#endif

namespace dci::mm::impl::stack
{
    Content::Content()
        : Base()
    {
#ifdef HAVE_VALGRIND
        auto& header = Base::header();
        header._valgrindId = VALGRIND_STACK_REGISTER(header._userspaceBegin, header._userspaceEnd);
#endif
    }

    Content::~Content()
    {
#ifdef HAVE_VALGRIND
        auto& header = Base::header();
        VALGRIND_STACK_DEREGISTER(header._valgrindId);
#endif
    }

    const Header& Content::header()
    {
        return Base::header();
    }

}
