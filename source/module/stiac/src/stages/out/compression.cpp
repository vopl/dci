// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "compression.hpp"
#include "../../protocol.hpp"

namespace dci::module::stiac::stages::out
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Compression::Compression(Protocol* protocol)
        : Base(protocol)
        , _zcs(nullptr)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Compression::~Compression()
    {
        if(_zcs)
        {
            ZSTD_freeCStream(_zcs);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint16 Compression::getWantedEmptyPrefix() const
    {
        return 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Compression::initialize()
    {
        if(_zcs)
        {
            return true;
        }

        _zcs = ZSTD_createCStream();
        if(!_zcs)
        {
            _protocol->internalError(this, "unable to create zstd instance");
            return false;
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes Compression::flushOutput()
    {
        if(!_zcs)
        {
            _protocol->internalError(this, "zstd uninitialized");
            return Bytes();
        }

        bytes::Alter src(_output.begin());

        Bytes output;
        bytes::Alter dst(output.begin());

        uint32 reserved = _output.empty() ? 0 : _wantedEmptyPrefix;
        if(reserved)
        {
            dst.advance(static_cast<int32>(reserved));
        }

        auto finalizer = [&]
        {
            if(reserved)
            {
                output.begin().remove(reserved);
            }

            return std::move(output);
        };

        while(!src.atEnd())
        {
            ZSTD_inBuffer inBuffer {src.continuousData(), src.continuousDataSize(), 0};

            uint32 writeBufferSize;
            ZSTD_outBuffer outBuffer {dst.prepareWriteBuffer(writeBufferSize), writeBufferSize, 0};

            size_t res = ZSTD_compressStream(_zcs, &outBuffer, &inBuffer);
            dst.commitWriteBuffer(static_cast<uint32>(outBuffer.pos));

            if(ZSTD_isError(res))
            {
                _protocol->internalError(this, std::string{"zstd compression failed: "} + ZSTD_getErrorName(res));

                return finalizer();
            }

            src.remove(static_cast<uint32>(inBuffer.pos));
        }

        for(;;)
        {
            uint32 writeBufferSize;
            ZSTD_outBuffer outBuffer {dst.prepareWriteBuffer(writeBufferSize), writeBufferSize, 0};

            size_t res = ZSTD_flushStream(_zcs, &outBuffer);
            dst.commitWriteBuffer(static_cast<uint32>(outBuffer.pos));

            if(!res)
            {
                break;
            }

            if(ZSTD_isError(res))
            {
                _protocol->internalError(this, std::string{"zstd compression failed: "} + ZSTD_getErrorName(res));

                return finalizer();
            }
        }

        return finalizer();
    }
}
