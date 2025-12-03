// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/mm/heap.hpp>
#include <cstdlib>
#include <cstring>
#include <map>

#include <boost/pool/singleton_pool.hpp>

/*
 * сейчас просто как заглушка
 * варианты:
 *      на штатной куче
 *      на кастомных кучах типа jemalloc, tcmalloc, etc. Даже более, многопоток не нужен - его можно вырезать
 *      на базе списков свободных блоков, boost::pool и подобные
 *      взять старую реализацию на базе магий с виртуальным пространством
 */

namespace dci::mm::heap
{
    void* alloc(std::size_t size)
    {
        return ::malloc(size);
    }

    void free(void* ptr)
    {
        return ::free(ptr);
    }

//    namespace
//    {
//        constexpr std::size_t classes = _sizeClassMax / _sizeClassStep + 1;
//        std::size_t cnts[classes] = {};
//        std::size_t opers{};

//        void dump()
//        {
//            std::string dump;
//            for(std::size_t idx{}; idx < classes; ++idx)
//            {
//                std::size_t cnt = cnts[idx];
//                if(cnt)
//                {
//                    std::size_t sc = idx * _sizeClassStep;
//                    if(sc < _sizeClassMin) sc = _sizeClassMin;
//                    dump += "    " + std::to_string(sc) + ": " + std::to_string(cnt) + "\n";
//                }
//            }
//            std::cerr<<"mm dump: \n"<<dump;
//        }

//        void incOper()
//        {
//            ++opers;

//            if(!(opers % 16384))
//            {
//                dump();
//            }
//        }

//        struct AtDestructionDumper
//        {
//            ~AtDestructionDumper()
//            {
//                dump();
//            }
//        } s_atDestructionDumper{};
//    }

    namespace details
    {
        struct Tag;
        template <std::size_t size>
        using Pool = boost::singleton_pool<Tag, size, boost::default_user_allocator_new_delete, boost::details::pool::null_mutex, 32, 1024>;

        template <std::size_t sizeClass> void* allocBySizeClass()
        {
//            ++cnts[sizeClass / _sizeClassStep];
//            incOper();

#ifndef NDEBUG
            void* ptr = Pool<sizeClass>::malloc();
            std::memset(ptr, 'A', sizeClass);
            return ptr;
#else
            return Pool<sizeClass>::malloc();
#endif
        }

        template <std::size_t sizeClass> void freeBySizeClass(void* ptr)
        {
//            --cnts[sizeClass / _sizeClassStep];
//            incOper();

#ifndef NDEBUG
            std::memset(ptr, 'F', sizeClass);
#endif
            return Pool<sizeClass>::free(ptr);
        }
    }

    static_assert(8 == dci::mm::heap::_sizeClassMin, "incompatible face");
    static_assert(4096 == dci::mm::heap::_sizeClassMax, "incompatible face");
    static_assert(16 == dci::mm::heap::_sizeClassStep, "incompatible face");

#define INSTANTIATEONESIZECLASS(sizeClass) template void* details::allocBySizeClass<sizeClass?sizeClass:8>(); template void details::freeBySizeClass<sizeClass?sizeClass:8>(void* ptr);

#define INSTANTIATEONESIZECLASS_x100(offset) \
    INSTANTIATEONESIZECLASS(offset + 0x00)\
    INSTANTIATEONESIZECLASS(offset + 0x10)\
    INSTANTIATEONESIZECLASS(offset + 0x20)\
    INSTANTIATEONESIZECLASS(offset + 0x30)\
    INSTANTIATEONESIZECLASS(offset + 0x40)\
    INSTANTIATEONESIZECLASS(offset + 0x50)\
    INSTANTIATEONESIZECLASS(offset + 0x60)\
    INSTANTIATEONESIZECLASS(offset + 0x70)\
    INSTANTIATEONESIZECLASS(offset + 0x80)\
    INSTANTIATEONESIZECLASS(offset + 0x90)\
    INSTANTIATEONESIZECLASS(offset + 0xa0)\
    INSTANTIATEONESIZECLASS(offset + 0xb0)\
    INSTANTIATEONESIZECLASS(offset + 0xc0)\
    INSTANTIATEONESIZECLASS(offset + 0xd0)\
    INSTANTIATEONESIZECLASS(offset + 0xe0)\
    INSTANTIATEONESIZECLASS(offset + 0xf0)


    INSTANTIATEONESIZECLASS_x100(0x000)
    INSTANTIATEONESIZECLASS_x100(0x100)
    INSTANTIATEONESIZECLASS_x100(0x200)
    INSTANTIATEONESIZECLASS_x100(0x300)
    INSTANTIATEONESIZECLASS_x100(0x400)
    INSTANTIATEONESIZECLASS_x100(0x500)
    INSTANTIATEONESIZECLASS_x100(0x600)
    INSTANTIATEONESIZECLASS_x100(0x700)
    INSTANTIATEONESIZECLASS_x100(0x800)
    INSTANTIATEONESIZECLASS_x100(0x900)
    INSTANTIATEONESIZECLASS_x100(0xa00)
    INSTANTIATEONESIZECLASS_x100(0xb00)
    INSTANTIATEONESIZECLASS_x100(0xc00)
    INSTANTIATEONESIZECLASS_x100(0xd00)
    INSTANTIATEONESIZECLASS_x100(0xe00)
    INSTANTIATEONESIZECLASS_x100(0xf00)

    INSTANTIATEONESIZECLASS(0x1000)
}
