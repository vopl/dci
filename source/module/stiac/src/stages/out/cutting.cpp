// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "cutting.hpp"
#include "../../protocol.hpp"

namespace dci::module::stiac::stages::out
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cutting::Cutting(Protocol* protocol, bool doIntegrityChecking)
        : Base(protocol)
        , _doIntegrityChecking(doIntegrityChecking)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint16 Cutting::getWantedEmptyPrefix() const
    {
        //приплюсовать 2 байта на заголовок
        return _wantedEmptyPrefix + 2;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cutting::input(Bytes&& msg)
    {
        dbgAssert(!msg.empty());

        //в 32 килобайта надо упихать заголовок(2байта), данные(maxChunkBodySize), контрольную сумму(8байт)
        const uint16 maxChunkBodySize =
                0x8000                             // 32к
                -2                                 // заголовок
                -(_doIntegrityChecking ? (8) : 0); // контрольная сумма

        while(msg.size() > maxChunkBodySize)
        {
            Bytes chunk;
            msg.begin().removeTo(chunk, maxChunkBodySize);
            pushChunk(std::move(chunk), false);
        }

        pushChunk(std::move(msg), true);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cutting::pushChunk(Bytes&& chunkData, bool finalize)
    {
        {
            //заголовок: 15 бит длины + 1 бит признак последнего куска в сообщении
            uint16 header = static_cast<uint16>(chunkData.size()) | (finalize ? uint16(1<<15) : uint16(0));
            header = stiac::serialization::fixEndian(header);

            //писать заголовок перед данными
            bytes::Alter alter(chunkData.begin());
            alter.advance(-2);
            alter.write(&header, 2);

            //дописать 8 байт контрольной суммы после данных
            if(_doIntegrityChecking)
            {
                Crc crc;
                while(!alter.atEnd())
                {
                    crc.process_bytes(alter.continuousData(), alter.continuousDataSize());
                    alter.advanceChunks(1);
                }

                uint64 checksum = stiac::serialization::fixEndian(crc.checksum());
                alter.write(&checksum, 8);
            }
        }

        _output.end().write(std::move(chunkData));
        if(finalize)
        {
            _protocol->linkHasOutput(this);
        }
    }
}
