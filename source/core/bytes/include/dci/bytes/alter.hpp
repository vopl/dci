// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../api.hpp"
#include <dci/bytes/implMetaInfo.hpp>
#include <dci/himpl.hpp>
#include <dci/primitives.hpp>
#include "cursor.hpp"
#include "chunk.hpp"

namespace dci
{
    class Bytes;
}

namespace dci::bytes
{
    class API_DCI_BYTES Alter
        : public himpl::FaceLayout<Alter, impl::Alter, Cursor>
    {
    public:
        Alter();
        Alter(const Alter& from);
        Alter(Alter&& from);
        ~Alter();

        Alter& operator=(const Alter& from);
        Alter& operator=(Alter&& from);

    public:
        void advance(int32 offset);//может нарастить пространство после конца или перед началом

        void* continuousData4Write();//инвалидируется при cow, модификациях контейнера

        byte* prepareWriteBuffer(/*out*/uint32& size);
        void commitWriteBuffer(uint32 size);

        void write(const void* src, uint32 size);
        uint32 write(const char* srcz);
        uint32 write(const Bytes& src, uint32 maxSize = ~uint32());
        uint32 write(Bytes&& src, uint32 maxSize = ~uint32());//буфера будут переиспользованы если никто более их не использует
        uint32 write(Cursor& src, uint32 maxSize = ~uint32());
        void write(Chunk* srcFirst, Chunk* srcLast, uint32 size);

        //укорачивает буфер данных на конце, курсор может быть затронут если он указывает на удаляемую позицию
        uint32 remove(uint32 maxSize = ~uint32());
        uint32 removeTo(void* dst, uint32 maxSize = ~uint32());
        uint32 removeTo(Bytes& dst, uint32 maxSize = ~uint32());
        uint32 removeTo(Alter& dst, uint32 maxSize = ~uint32());
    };

}
