// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "downloader.hpp"
#include "recvBuffer.hpp"
#include "../base.hpp"

namespace dci::module::ppn::service::aup_legacy_since_2025_04::consumer::base
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Downloader::Downloader(Base* b, const Oid& oid, int priority)
        : _b{b}
        , _oid{oid}
        , _priority{priority}
    {
        cmt::spawn() += _taskOwner * [this]
        {
            worker();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Downloader::~Downloader()
    {
        for(Remote* r : std::exchange(_candidates, {}))
        {
            r->uninvolve(this);
        }

        _b->quota().done(this);
        _taskOwner.stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Downloader::involve(Remote* r)
    {
        _candidates.insert(r);
        _awaker.raise();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Downloader::uninvolve(Remote* r)
    {
        _candidates.erase(r);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Oid& Downloader::oid() const
    {
        return _oid;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    int Downloader::priority() const
    {
        return _priority;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Downloader::canWork()
    {
        _canWork = true;
        _awaker.raise();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Downloader::worker()
    {
        bool done = false;

        {
            struct Transfer : std::enable_shared_from_this<Transfer>, mm::heap::Allocable<Transfer>
            {
                sbs::Owner                                  _sbsOwner;
                api_legacy_since_2025_04::BlobTransfer<>    _api;

                Transfer()
                    : _api{idl::interface::Initializer{}}
                {
                }

                ~Transfer()
                {
                    close();
                }

                void close()
                {
                    _sbsOwner.flush();
                    _api.reset();
                }
            };
            using TransferPtr = std::shared_ptr<Transfer>;
            std::set<TransferPtr> transfersReady;
            std::set<TransferPtr> transfersWait;

            auto updateTransfer = [&](const TransferPtr& t, api_legacy_since_2025_04::BlobStatus bs)
            {
                if(!t->_api)
                {
                    transfersReady.erase(t);
                    transfersWait.erase(t);
                    return;
                }

                switch(bs)
                {
                case api_legacy_since_2025_04::BlobStatus::present:
                    transfersReady.insert(t);
                    transfersWait.erase(t);
                    break;
                case api_legacy_since_2025_04::BlobStatus::missingAndWanted:
                    transfersReady.erase(t);
                    transfersWait.insert(t);
                    break;
                case api_legacy_since_2025_04::BlobStatus::missingAndUnwanted:
                default:
                    t->close();
                    transfersReady.erase(t);
                    transfersWait.erase(t);
                    break;
                }
            };

            const uint32 granulaSize = 1024 * 128;
            RecvBuffer recvBuffer{granulaSize};

            auto workRaii = [this]
            {
                if(!_canWork)
                {
                    _b->quota().ready(this);
                }

                while(!_canWork)
                {
                    _awaker.wait();
                }

                _canWork = false;

                return utils::AtScopeExit{[this]
                {
                    _b->quota().done(this);
                }};
            };

            for(;;)
            {
                LOGD(_b->_name<<": start0 "<<utils::b2h(_oid));
                if(!transfersReady.empty())
                {
                    LOGD(_b->_name<<": start1 "<<utils::b2h(_oid));
                    auto wl = workRaii();
                    LOGD(_b->_name<<": start2 "<<utils::b2h(_oid));

                    if(!transfersReady.empty())
                    {
                        LOGD(_b->_name<<": start3 "<<utils::b2h(_oid));
                        TransferPtr transfer = *transfersReady.begin();
                        dbgAssert(transfer->_api);

                        LOGD(_b->_name<<": blob get piece: "<<utils::b2h(_oid)<<", "<<recvBuffer.payloadSize()<<", "<<granulaSize+0);
                        auto resf = transfer->_api->getPiece(recvBuffer.payloadSize(), granulaSize+0);
                        resf.wait();

                        if(resf.resolvedValue())
                        {
                            auto res = resf.detachValue();

                            api_legacy_since_2025_04::BlobStatus bs = std::get<0>(res);
                            LOGD(_b->_name<<": blob got piece: "<<utils::b2h(_oid)<<", "<<recvBuffer.payloadSize()<<", "<<granulaSize+0 << ", bs " << static_cast<int>(bs));

                            updateTransfer(transfer, bs);
                            if(api_legacy_since_2025_04::BlobStatus::present != bs)
                            {
                                continue;
                            }

                            Bytes&& recv = std::move(res).get<1>();
                            uint32 recvSize = recv.size();

                            if(!recvBuffer.push(std::move(recv)))
                            {
                                recvBuffer.reset();
                                LOGW(_b->_name<<": unable to emplace blob: "<<utils::b2h(_oid));
                                updateTransfer(transfer, api_legacy_since_2025_04::BlobStatus::missingAndUnwanted);
                                continue;
                            }

                            if(recvSize != granulaSize)
                            {
                                utils::AtScopeExit se{[&]
                                {
                                    recvBuffer.reset();
                                }};

                                std::size_t recvBufferPayloadSize = recvBuffer.payloadSize();
                                if(_b->ready(this, recvBuffer))
                                {
                                    LOGI(_b->_name<<": blob transfer complete: "<<utils::b2h(_oid)<<", "<<recvBufferPayloadSize<<" bytes");
                                    done = true;
                                    break;
                                }

                                //ауп не принял полученный блоб, бросить его и попробовать еще раз
                                LOGW(_b->_name<<": blob unaccepted: "<<utils::b2h(_oid)<<", "<<recvBufferPayloadSize<<" bytes");
                            }
                        }
                        else if(resf.resolvedException())
                        {
                            recvBuffer.reset();
                            LOGW(_b->_name<<": blob transfer failed: "<<utils::b2h(_oid)<<", "<<exception::toString(resf.detachException()));
                            updateTransfer(transfer, api_legacy_since_2025_04::BlobStatus::missingAndUnwanted);
                            continue;
                        }
                        else //if(resf.resolvedCancel())
                        {
                            recvBuffer.reset();
                            LOGW(_b->_name<<": blob transfer canceled: "<<utils::b2h(_oid));
                            updateTransfer(transfer, api_legacy_since_2025_04::BlobStatus::missingAndUnwanted);
                            continue;
                        }
                    }

                    continue;
                }

                if(_candidates.empty())
                {
                    _awaker.wait();
                    continue;
                }

                auto wl = workRaii();

                if(!_candidates.empty())
                {

                    Remote* r = *_candidates.begin();
                    _candidates.erase(_candidates.begin());
                    r->uninvolve(this);


                    TransferPtr transfer {std::make_shared<Transfer>()};

                    transfer->_api.involvedChanged() += transfer->_sbsOwner * [this,&updateTransfer,raw=transfer.get()](bool v)
                    {
                        if(!v)
                        {
                            updateTransfer(raw->shared_from_this(), api_legacy_since_2025_04::BlobStatus::missingAndUnwanted);
                            _awaker.raise();
                        }
                    };
                    transfer->_api->statusChanged() += transfer->_sbsOwner * [this,&updateTransfer,raw=transfer.get()](api_legacy_since_2025_04::BlobStatus bs)
                    {
                        updateTransfer(raw->shared_from_this(), bs);
                        _awaker.raise();
                    };

                    LOGI(_b->_name<<": blob transfer start: "<<utils::b2h(_oid));
                    updateTransfer(transfer, api_legacy_since_2025_04::BlobStatus::present);
                    r->api()->startBlobTransfer(_oid, transfer->_api.opposite());
                }
            }
        }

        if(done)
        {
            cmt::task::current().ownTo(nullptr);
            _b->done(this);
        }
    }
}
