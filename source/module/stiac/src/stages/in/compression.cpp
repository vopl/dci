// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "compression.hpp"
#include "../../protocol.hpp"

namespace dci::module::stiac::stages::in
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Compression::Compression(Protocol* protocol)
        : Base(protocol)
        , _zds(nullptr)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Compression::~Compression()
    {
        if(_zds)
        {
            ZSTD_freeDStream(_zds);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Compression::initialize()
    {
        if(_zds)
        {
            return true;
        }

        _zds = ZSTD_createDStream();
        if(!_zds)
        {
            _protocol->internalError(this, "unable to create zstd instance");
            return false;
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes Compression::flushOutput()
    {
        if(!_zds)
        {
            _protocol->internalError(this, "zstd uninitialized");
            return Bytes();
        }

        bytes::Alter src(_output.begin());

        Bytes output;
        bytes::Alter dst(output.begin());

        for(;;)
        {
            ZSTD_inBuffer inBuffer {src.continuousData(), src.continuousDataSize(), 0};

            uint32 writeBufferSize;
            ZSTD_outBuffer outBuffer {dst.prepareWriteBuffer(writeBufferSize), writeBufferSize, 0};

            size_t res = ZSTD_decompressStream(_zds, &outBuffer, &inBuffer);
            dst.commitWriteBuffer(static_cast<uint32>(outBuffer.pos));

            if(ZSTD_isError(res))
            {
                _protocol->internalError(this, std::string{"zstd decompression failed: "} + ZSTD_getErrorName(res));
                return output;
            }

            src.remove(static_cast<uint32>(inBuffer.pos));

            if(src.atEnd())
            {
                if(outBuffer.pos < outBuffer.size)
                {
                    return output;
                }

                break;
            }
        }

        ZSTD_inBuffer inBufferNull {nullptr, 0, 0};
        for(;;)
        {
            uint32 writeBufferSize;
            ZSTD_outBuffer outBuffer {dst.prepareWriteBuffer(writeBufferSize), writeBufferSize, 0};

            size_t res = ZSTD_decompressStream(_zds, &outBuffer, &inBufferNull);
            dst.commitWriteBuffer(static_cast<uint32>(outBuffer.pos));

            if(ZSTD_isError(res))
            {
                _protocol->internalError(this, std::string{"zstd decompression failed: "} + ZSTD_getErrorName(res));
                return output;
            }

            if(outBuffer.pos < outBuffer.size)
            {
                return output;
            }
        }

        dbgWarn("never here");
        return output;
    }
}
