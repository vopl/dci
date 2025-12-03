// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <utility>

namespace dci::himpl
{
    class ImplDestructorCaller
    {
    public:
        ~ImplDestructorCaller() noexcept
        {
            _destructed = true;
        }

        template <class T>
        void operator()(T* o) noexcept(noexcept(std::declval<T>().~T()))
        {
            //тут неопределенное поведение - обращение к полю объекта после отработки деструктора (но до разрушения интерфейсного объекта, который держит область памяти)
            if(!_destructed)
            {
                o->~T();
            }
        }

    private:
        bool _destructed = false;
    };
}
