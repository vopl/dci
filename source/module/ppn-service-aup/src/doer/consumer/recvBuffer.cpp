/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "recvBuffer.hpp"

namespace dci::module::ppn::service::aup::doer::consumer
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    RecvBuffer::RecvBuffer(uint32 ramBound)
        : _ramBound{ramBound}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    RecvBuffer::~RecvBuffer()
    {
        reset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool RecvBuffer::push(Bytes&& payload)
    {
        if(_fsPayload)
        {
            dbgAssert(_ramPayload.empty());
            _payloadSize += payload.size();
            return pushFs(std::move(payload));
        }

        if(_payloadSize + payload.size() >= _ramBound)
        {
            if(!pushFs(std::move(_ramPayload)))
                return false;

            _payloadSize += payload.size();
            if(!pushFs(std::move(payload)))
                return false;
        }
        else
        {
            _payloadSize += payload.size();
            _ramPayload.end().write(std::move(payload));
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 RecvBuffer::payloadSize()
    {
        return _payloadSize;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool RecvBuffer::hasFile()
    {
        return !!_fsPayload;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes RecvBuffer::detachBytes()
    {
        utils::AtScopeExit se{[this]
        {
            reset();
        }};

        if(!_fsPayload)
        {
            return std::move(_ramPayload);
        }

        rewind(_fsPayload);
        Bytes res;

        {
            bytes::Alter a{res.end()};

            for(;;)
            {
                uint32 bsize;
                void* buf = a.prepareWriteBuffer(bsize);
                size_t rsize = fread(buf, 1, bsize, _fsPayload);
                if(rsize)
                {
                    a.commitWriteBuffer(static_cast<uint32>(rsize));
                }

                if(rsize != bsize)
                {
                    break;
                }
            }
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::FILE* RecvBuffer::getFile()
    {
        if(!_fsPayload)
        {
            if(!pushFs(std::move(_ramPayload)))
            {
                return {};
            }
        }

        dbgAssert(_fsPayload);
        return _fsPayload;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void RecvBuffer::reset()
    {
        _payloadSize = 0;

        _ramPayload.clear();

        if(_fsPayload)
        {
            std::fclose(_fsPayload);
            _fsPayload = nullptr;
        }

        if(!_fsPayloadPath.empty())
        {
            ::unlink(_fsPayloadPath.c_str());
            _fsPayloadPath = {};
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool RecvBuffer::pushFs(Bytes&& payload)
    {
        if(!_fsPayload)
        {
            std::error_code ec;
            {
                std::filesystem::path tmpName = std::filesystem::temp_directory_path(ec);
                if(ec)
                {
                    LOGE("consumer unable to obtain temporary directory name: "<<ec);
                    return false;
                }

                tmpName /= "dci-ppn-aup.XXXXXX";
                _fsPayloadPath = tmpName.string();
            }

            int fd = mkstemp(_fsPayloadPath.data());
            if(0 > fd)
            {
                LOGE("consumer unable to create temporary file `"<<_fsPayloadPath<<"`: "<<std::error_code{errno, std::generic_category()});
                return false;
            }

            _fsPayload = fdopen(fd, "wb+");
            if(!_fsPayload)
            {
                ::close(fd);
                ::unlink(_fsPayloadPath.c_str());
                _fsPayloadPath = {};

                LOGE("consumer unable to fdopen temporary file `"<<_fsPayloadPath<<"`: "<<std::error_code{errno, std::generic_category()});
                return false;
            }
        }

        bytes::Cursor c{payload.begin()};
        while(c.size())
        {
            uint32 size = c.continuousDataSize();
            if(size != fwrite(c.continuousData(), 1, size, _fsPayload))
            {
                LOGE("consumer unable to write temporary file: "<<std::error_code{errno, std::generic_category()});
                return false;
            }
            c.advanceChunks(1);
        }
        payload.clear();
        return true;
    }

}
