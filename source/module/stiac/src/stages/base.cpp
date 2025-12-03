// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "base.hpp"
#include "../protocol.hpp"

namespace dci::module::stiac::stages
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Base::Base(Protocol* protocol)
        : _protocol(protocol)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Base::~Base()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Base::initialize()
    {
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Base::setIndexInChain(std::size_t index)
    {
        _indexInChain = index;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Base::getIndexInChain() const
    {
        return _indexInChain;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Base::setWantedEmptyPrefix(uint16 size)
    {
        _wantedEmptyPrefix = size;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint16 Base::getWantedEmptyPrefix() const
    {
        return _wantedEmptyPrefix + 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Base::input(Bytes&& msg)
    {
        //дефолтная реализация проводит трафик 1:1 со входа на выход
        accumulateOutput(std::move(msg));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Base::hasOutput() const
    {
        return !_output.empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes Base::flushOutput()
    {
        return std::move(_output);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes& Base::outputBuffer()
    {
        return _output;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Base::accumulateOutput(Bytes&& msg)
    {
        if(msg.empty())
        {
            return;
        }

        _output.end().write(std::move(msg));
        _protocol->linkHasOutput(this);
    }
}
