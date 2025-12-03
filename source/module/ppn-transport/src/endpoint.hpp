// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::transport
{
    template <class Iface>
    struct Endpoint
    {
        Iface           _instance;
        sbs::Owner      _sbsOwner;

        api::Address        _address;
        std::string         _addressSchema;
        utils::ip::Scope    _addressIpScope {};

        real64          _cost       {};
        real64          _rtt        {};
        real64          _bandwidth  {};

        uint64          _readyMask {};
        cmt::Event      _ready;

        ~Endpoint()
        {
            _sbsOwner.flush();
            _instance.reset();
        }

        void updateReady(uint64 flags)
        {
            _readyMask |= flags;
            if(_readyMask & 0xf)
            {
                _ready.raise();
            }
        }

        void updateAddress()
        {
            _addressSchema = utils::uri::scheme(_address.value);

            if(_addressSchema.starts_with("tcp"))
            {
                _addressIpScope = utils::ip::scope(utils::uri::hostPort(_address.value));
            }
            else
            {
                _addressIpScope = utils::ip::Scope::null;
            }
        }

        void subscribe(Iface&& instance)
        {
            unsubscribe();
            _instance = std::move(instance);

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            _instance->address().then() += _sbsOwner * [this](auto f)
            {
                if(f.resolvedValue())
                {
                    _address = f.detachValue();
                    updateAddress();
                    updateReady(1);
                }
            };

            _instance->addressChanged() += _sbsOwner * [this](auto&& v)
            {
                _address = std::forward<decltype(v)>(v);
                updateAddress();
                updateReady(1);
            };

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            _instance->cost().then() += _sbsOwner * [this](auto f)
            {
                if(f.resolvedValue())
                {
                    _cost = f.detachValue();
                    updateReady(2);
                }
            };

            _instance->costChanged() += _sbsOwner * [this](auto&& v)
            {
                _cost = std::forward<decltype(v)>(v);
                updateReady(2);
            };

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            _instance->rtt().then() += _sbsOwner * [this](auto f)
            {
                if(f.resolvedValue())
                {
                    _rtt = f.detachValue();
                    updateReady(4);
                }
            };

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            _instance->rttChanged() += _sbsOwner * [this](auto&& v)
            {
                _rtt = std::forward<decltype(v)>(v);
                updateReady(4);
            };

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            _instance->bandwidth().then() += _sbsOwner * [this](auto f)
            {
                if(f.resolvedValue())
                {
                    _bandwidth = f.detachValue();
                    updateReady(8);
                }
            };

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            _instance->bandwidthChanged() += _sbsOwner * [this](auto&& v)
            {
                _bandwidth = std::forward<decltype(v)>(v);
                updateReady(8);
            };
        }

        void unsubscribe()
        {
            _sbsOwner.flush();
        }
    };
}
