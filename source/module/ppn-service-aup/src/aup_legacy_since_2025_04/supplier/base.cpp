/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "base.hpp"
#include "../../aup.hpp"

namespace dci::module::ppn::service::aup_legacy_since_2025_04::supplier
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Base::Base()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Base::~Base()
    {
        _sbsOwner.flush();
        _transfers.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Base::startOne(const Oid& oid, api_legacy_since_2025_04::BlobTransfer<>::Opposite&& bt)
    {
        Transfer& transfer = _transfers.try_emplace(
                                 oid,
                                 this, oid).first->second;
        return transfer.startOne(std::move(bt));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Base::transferBecomesEmpty(const Oid& oid)
    {
        _transfers.erase(oid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Base::Transfer::Transfer(Base* base, const Oid& oid)
        : _base{base}
        , _oid{oid}
        , _status{_base->getStatus(_oid)}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Base::Transfer::~Transfer()
    {
        _sbsOwner.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Base::Transfer::startOne(api_legacy_since_2025_04::BlobTransfer<>::Opposite&& bt)
    {
        dbgAssert(_ifaces.end() == _ifaces.find(bt));

        bt.involvedChanged() += _sbsOwner * [this, bt=bt.weak()](bool v)
        {
            if(!v)
            {
                _ifaces.erase(bt);
                if(_ifaces.empty())
                {
                    _base->transferBecomesEmpty(_oid);
                }
            }
        };

        //in getPiece(uint32 offset, uint32 size) -> tuple<BlobStatus, bytes>;
        bt->getPiece() += _sbsOwner * [this](uint32 offset, uint32 size)
        {
            LOGD("getPiece for " << utils::b2h(_oid) << ", " << offset << ", " << size);
            if(api_legacy_since_2025_04::BlobStatus::present == _status)
            {
                try
                {
                    Bytes piece = _base->getPiece(_oid, offset, size);
                    LOGD("has piece " << piece.size());
                    return cmt::readyFuture(Tuple{_status, std::move(piece)});
                }
                catch(...)
                {
                    LOGW("supplier: unable to fetch blob piece: "<<exception::toString(std::current_exception()));
                    return cmt::readyFuture<Tuple<api_legacy_since_2025_04::BlobStatus, Bytes>>(exception::buildInstance<api_legacy_since_2025_04::Error>("unable to supply piece requested"));
                }
            }

            LOGD("has no piece");
            return cmt::readyFuture(Tuple{_status, Bytes{}});
        };

        _ifaces.emplace(std::move(bt));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Base::Transfer::updateStatus()
    {
        api_legacy_since_2025_04::BlobStatus status = _base->getStatus(_oid);
        if(_status != status)
        {
            _status = status;

            //out statusChanged(BlobStatus);
            for(api_legacy_since_2025_04::BlobTransfer<>::Opposite iface : _ifaces)
            {
                iface->statusChanged(_status);
            }
        }
    }
}

