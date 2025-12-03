// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"

namespace dci::module::www::http::compress
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class User>
    struct IoRoller
    {
        User* user()
        {
            return static_cast<User*>(this);
        }

        IoRoller(Bytes& src, Bytes& dst)
            : _src{src.begin()}
            , _dst{dst.end()}
        {
            user()->setIn(nullptr, 0);
            user()->setOut(nullptr, 0);
        }

        ~IoRoller()
        {
            {
                if(_srcChunkSize > user()->getInSize())
                {
                    _src.remove(_srcChunkSize - user()->getInSize());
                    //_srcChunkSize = 0;
                }

                user()->setIn(nullptr, 0);
                //_srcAtFinish = true;
            }

            {
                if(_dstChunkSize > user()->getOutSize())
                    _dst.commitWriteBuffer(_dstChunkSize - user()->getOutSize());
                //_dstChunkSize = 0;

                user()->setOut(nullptr, 0);
            }
        }

        bool srcAtFinish() const
        {
            return _srcAtFinish;
        }

        void step()
        {
            if(!user()->getInSize())
            {
                if(_srcChunkSize)
                    _src.remove(_srcChunkSize);

                if(_srcAtFinish)
                {
                    _srcChunkSize = 0;
                    user()->setIn(nullptr, 0);
                }
                else
                {
                    _srcChunkSize = _src.continuousDataSize();

                    if(_srcChunkSize)
                    {
                        user()->setIn(_src.continuousData(), _srcChunkSize);
                        _srcAtFinish = _srcChunkSize == _src.size();
                    }
                    else
                    {
                        user()->setIn(nullptr, 0);
                        _srcAtFinish = true;
                    }
                }
            }

            if(!user()->getOutSize())
            {
                if(_dstChunkSize)
                    _dst.commitWriteBuffer(_dstChunkSize);

                void* buf = _dst.prepareWriteBuffer(_dstChunkSize);
                user()->setOut(buf, _dstChunkSize);
            }
        }

    private:
        bytes::Alter    _src;
        uint32          _srcChunkSize{};
        bool            _srcAtFinish{};

        bytes::Alter    _dst;
        uint32          _dstChunkSize{};
    };
}
