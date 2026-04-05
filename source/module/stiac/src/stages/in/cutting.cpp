/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "cutting.hpp"
#include "../../protocol.hpp"

namespace dci::module::stiac::stages::in
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cutting::Cutting(Protocol* protocol, bool doIntegrityChecking)
        : Base(protocol)
        , _doIntegrityChecking(doIntegrityChecking)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cutting::input(Bytes&& msg)
    {
        _input.end().write(std::move(msg));

        const uint16 checksumSize = _doIntegrityChecking ? (8) : 0;

        for(;;)
        {
            if(!_chunkSize)
            {
                const uint16 minChunkSize =
                        2               // заголовок
                        +checksumSize;  // контрольная сумма

                if(_input.size() < minChunkSize)
                {
                    //не хватает данных заголовка
                    return;
                }

                //вычитать заголовок и переработать его
                uint16 header;
                _input.begin().removeTo(&header, 2);
                header = stiac::serialization::fixEndian(header);

                _chunkSize = header & 0x7fff;
                _chunkFinal = header & 0x8000 ? true : false;
            }

            if(_input.size() < _chunkSize + checksumSize)
            {
                //не хватает данных тела и контрольной суммы
                return;
            }

            Bytes chunk;
            bytes::Alter src(_input.begin());

            //вычитать тело и контрольную сумму
            src.removeTo(chunk, _chunkSize);
            dbgAssert(chunk.size() == _chunkSize);

            //пересчитать контрольную сумму и провалидировать
            if(_doIntegrityChecking)
            {
                uint64 checksum;
                src.removeTo(&checksum, 8);
                checksum = stiac::serialization::fixEndian(checksum);

                bytes::Cursor c(chunk.begin());
                Crc crc;
                while(!c.atEnd())
                {
                    crc.process_bytes(c.continuousData(), c.continuousDataSize());
                    c.advanceChunks(1);
                }

                if(crc.checksum() != checksum)
                {
                    _protocol->integrityViolation(this);
                }
            }

            //накапливать куски в цельное сообщение
            _message.end().write(std::move(chunk));

            if(_chunkFinal)
            {
                Base::input(std::move(_message));
            }

            _chunkSize = 0;
            _chunkFinal = false;
        }
    }
}
