/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "symmetric.hpp"
#include "../protocol.hpp"

namespace dci::module::stiac::crypto
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Symmetric::~Symmetric()
    {
        cleanMemoryUnder(_hs);

        cleanMemoryUnder(_keySetted);

        _aead.clear();
        _hashMixer.clear();
        _mac4kdf.clear();

        cleanMemoryUnder(_nonce);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Symmetric::setHandshake(Handshake* hs)
    {
        _hs = hs;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Symmetric::resetState()
    {
        _keySetted = false;
        _aead.clear();
        _hash.fill(0);
        _chainingKey.fill(0);
        _nonce._counter = 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Symmetric::mixHash(const Bytes& material)
    {
        _hashMixer.add(_hash.data(), _hash.size());

        bytes::Cursor c(material.begin());
        while(!c.atEnd())
        {
            _hashMixer.add(c.continuousData(), c.continuousDataSize());
            c.advanceChunks(1);
        }

        _hashMixer.finish(_hash.data());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Symmetric::mixHash(const char* cszMaterial)
    {
        mixHash(cszMaterial, static_cast<uint32>(strlen(cszMaterial)));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Symmetric::mixHash(const void* materialData, uint32 materialSize)
    {
        _hashMixer.add(_hash.data(), _hash.size());
        _hashMixer.add(materialData, materialSize);
        _hashMixer.finish(_hash.data());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Symmetric::mixKey(const uint8 material[_keySize])
    {
        _mac4kdf.setKey(_chainingKey.data(), _chainingKey.size());
        //_mac4kdf.start();
        _mac4kdf.add(material, _keySize);
        uint8 tmp[_hashSize];
        _mac4kdf.finish(tmp);

        _mac4kdf.setKey(tmp, sizeof(tmp));
        //_mac4kdf.start();
        const uint8 one[1] = {1};
        _mac4kdf.add(one, sizeof(one));
        uint8 key[_hashSize];
        _mac4kdf.finish(key);

        _mac4kdf.setKey(tmp, sizeof(tmp));
        //_mac4kdf.start();
        _mac4kdf.add(key, sizeof(key));
        const uint8 two[1] = {2};
        _mac4kdf.add(two, sizeof(two));
        _mac4kdf.finish(_chainingKey.data());

        _aead.setKey(key, _keySize);
        _nonce._counter = 0;

        _keySetted = true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Symmetric::messageStart()
    {
        Nonce nonce = _nonce;
        _nonce._counter++;
        nonce._counter = stiac::serialization::fixEndian(nonce._counter);

        _aead.setAd(_hash.data(), _hash.size());
        _aead.start(nonce._raw, sizeof(nonce));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Symmetric::mixHashStart()
    {
        _hashMixer.add(_hash.data(), _hash.size());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Symmetric::messageEncipher(void* data, uint32 size)
    {
        _aead.encipher(data, data, size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Symmetric::messageDecipher(void* data, uint32 size)
    {
        _aead.decipher(data, data, size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Symmetric::mixHashUpdate(const void* data, uint32 size)
    {
        _hashMixer.add(data, size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Symmetric::messageEncipherFinish(void* macOut)
    {
        _aead.encipherFinish(macOut);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Symmetric::messageDecipherFinish(const void* macIn)
    {
        return _aead.decipherFinish(macIn);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Symmetric::mixHashFinish()
    {
        _hashMixer.finish(_hash.data());
    }
}
