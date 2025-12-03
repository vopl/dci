// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "localEdge.hpp"
#include "protocol.hpp"

namespace dci::module::stiac
{
    using namespace localEdge;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    LocalEdge::LocalEdge(Protocol* protocol, const api::LocalEdge<>::Opposite& interface)
        : stages::Base(protocol)
        , localEdge::Output(stages::Base::outputBuffer())
        , _interface(interface)
    {
        _duty.initialize(this, link::Id::null);

        _interface.involvedChanged() += this * [this](bool v)
        {
            if(!v)
            {
                _protocol->localEdgeWantRemove(this);
            }
        };

        //in state() -> localEdge::State;
        _interface->state() += this * [this]
        {
            return cmt::readyFuture(_state);
        };

        //out stateChanged(localEdge::State);
        //out failed(exception);

        //in put(interface instance) -> void;
        _interface->put() += this * [this](Interface&& instance)
        {
            switch(_state)
            {
            case apil::State::work:
            case apil::State::pause:
                break;

            default:
                return cmt::readyFuture<None>(std::make_exception_ptr(apil::BadState()));
            }

            if(!instance)
            {
                return cmt::readyFuture<None>(std::make_exception_ptr(apil::BadArgument("target interface must not be null")));
            }

            ILid identifier = instance.mdLid();
            return _duty.putInterface(std::move(instance), identifier);
        };

        //in putConcrete(interface instance, id identifier) -> void;
        _interface->putConcrete() += this * [this](Interface&& instance, ILid identifier)
        {
            switch(_state)
            {
            case apil::State::work:
            case apil::State::pause:
                break;

            default:
                return cmt::readyFuture<None>(std::make_exception_ptr(apil::BadState()));
            }

            if(!instance)
            {
                return cmt::readyFuture<None>(std::make_exception_ptr(apil::BadArgument("target interface must not be null")));
            }

            return _duty.putInterface(std::move(instance), identifier);
        };

        //in optimisticPut(interface instance);
        _interface->optimisticPut() += this * [this](Interface&& instance)
        {
            switch(_state)
            {
            case apil::State::work:
            case apil::State::pause:
                break;

            default:
                return;
            }

            if(!instance)
            {
                return;
            }

            ILid identifier = instance.mdLid();
            return _duty.optimisticPutInterface(std::move(instance), identifier);
        };

        //in optimisticPutConcrete(interface instance, id identifier);
        _interface->optimisticPutConcrete() += this * [this](Interface&& instance, ILid identifier)
        {
            switch(_state)
            {
            case apil::State::work:
            case apil::State::pause:
                break;

            default:
                return;
            }

            if(!instance)
            {
                return;
            }

            return _duty.optimisticPutInterface(std::move(instance), identifier);
        };

        //out got(interface instance) -> void;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    LocalEdge::~LocalEdge()
    {
        sbs::Owner::flush();
        _localLinks.deinitialize();
        _remoteLinks.deinitialize();
        _duty.deinitialize();
        _state = apil::State::null;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void LocalEdge::start()
    {
        if(apil::State::work != _state)
        {
            _state = apil::State::work;
            _interface->stateChanged(_state);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void LocalEdge::pause()
    {
        if(apil::State::work == _state)
        {
            _state = apil::State::pause;
            _interface->stateChanged(_state);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint16 LocalEdge::getWantedEmptyPrefix() const
    {
        return 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void LocalEdge::input(Bytes&& msg)
    {
        Input::append(std::move(msg));

        if(_inputProcessingActive)
        {
            return;
        }

        _inputProcessingActive = true;

        try
        {
            while(apil::State::work == _state && !Input::empty())
            {
                link::Source source = Input::makeSource();

                link::MirroredId mirroredId;
                source >> mirroredId;

                link::Id id = linkIdCast<link::Id>(mirroredId);

                link::Base* link;
                if(linkIsNull(id))
                {
                    link = &_duty;
                }
                else if(linkIsLocal(id))
                {
                    link::LocalId localId = linkIdCast<link::LocalId>(id);
                    link = _localLinks.get(localId);
                }
                else //if(linkIsRemote(id))
                {
                    dbgAssert(linkIsRemote(id));

                    link::RemoteId remoteId = linkIdCast<link::RemoteId>(id);
                    link = _remoteLinks.get(remoteId);
                }

                if(!link)
                {
                    source.fail("bad link requested");
                    return;
                }

                link->input(source);

                dbgAssert(source.finalized());
            }

            _inputProcessingActive = false;
        }
        catch(link::source::Fail& fail)
        {
            _inputProcessingActive = false;

            _state = apil::State::fail;
            api::LocalEdge<>::Opposite interface = _interface;
            _protocol->localEdgeFail(this, fail.details());

            if(interface)
            {
                interface->failed(std::make_exception_ptr(apil::BadInput(fail.details())));
                interface->stateChanged(apil::State::fail);
            }
        }
        catch(...)
        {
            dbgWarn("never here");
            _inputProcessingActive = false;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes LocalEdge::flushOutput()
    {
        return stages::Base::flushOutput();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    link::Sink LocalEdge::makeSink(link::Id id)
    {
        link::Sink sink = Output::makeSink(_wantedEmptyPrefix);
        sink << id;
        return sink;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void LocalEdge::linkUninvolved(link::Id id, int uf)
    {
        if(apil::State::null == _state)
        {
            dbgWarn("must never happend");
            return;
        }

        auto processor = [this](int uf, auto& links, auto id)
        {
            dbgAssert(!!links.get(id));

            {
                bool res = false;

                if(!res && (Hub4Link::uf_remove & uf))
                {
                    res = links.remove(id);
                }

                if(!res && (Hub4Link::uf_endRemove & uf))
                {
                    res = links.endRemove(id);
                }

                if(!res && (Hub4Link::uf_beginRemove & uf))
                {
                    res = links.beginRemove(id);
                }

                dbgAssert(res);
                (void)res;
            }

            if(uf & Hub4Link::uf_sendBegin)
            {
                _duty.linkBeginRemove(id);
            }
            if(uf & Hub4Link::uf_sendEnd)
            {
                _duty.linkEndRemove(id);
            }
        };

        if(linkIsLocal(id))
        {
            processor(uf, _localLinks, linkIdCast<link::LocalId>(id));
        }
        else
        {
            dbgAssert(linkIsRemote(id));
            processor(uf, _remoteLinks, linkIdCast<link::RemoteId>(id));
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool LocalEdge::emplaceLink(link::BasePtr&& link, link::RemoteId remoteId)
    {
        link::Base* linkPtrCopy = link.get();
        bool res = _remoteLinks.emplace(std::move(link), remoteId);

        if(res)
        {
            dbgAssert(linkPtrCopy == _remoteLinks.get(remoteId));
            linkPtrCopy->initialize(this, linkIdCast<link::Id>(remoteId));
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void LocalEdge::finalize(link::Source& source, bytes::Alter&& buffer)
    {
        return Input::finalize(source, std::move(buffer));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    link::LocalId LocalEdge::emplaceLink(link::BasePtr&& link)
    {
        link::Base* linkPtrCopy = link.get();
        link::LocalId localId = _localLinks.emplace(std::move(link));
        dbgAssert(linkPtrCopy == _localLinks.get(localId));
        linkPtrCopy->initialize(this, linkIdCast<link::Id>(localId));

        return localId;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void LocalEdge::finalize(link::Sink& sink, bytes::Alter&& buffer)
    {
        Output::finalize(sink, std::move(buffer));
        if(apil::State::work == _state)
        {
            _protocol->linkHasOutput(this);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<None> LocalEdge::oppositePutInterface(Interface&& interface)
    {
        switch(_state)
        {
        case apil::State::work:
            return _interface->got(std::move(interface));

        default:
            throw link::source::Fail("bad state");
            break;
        }

        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void LocalEdge::oppositeOptimisticPutInterface(Interface&& interface)
    {
        if(apil::State::work == _state)
        {
            _interface->got(std::move(interface)).then() += []
            {
                //fake continuation for keep future alive
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void LocalEdge::oppositeLinkBeginRemove(link::LocalId localId)
    {
        if(apil::State::work != _state)
        {
            throw link::source::Fail("bad state");
        }

        if(_localLinks.remove(localId))
        {
            _duty.linkEndRemove(localId);
            return;
        }

        if(_localLinks.endRemove(localId))
        {
            return;
        }

        throw link::source::Fail("bad local link id marked to delete");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void LocalEdge::oppositeLinkBeginRemove(link::RemoteId remoteId)
    {
        if(apil::State::work != _state)
        {
            throw link::source::Fail("bad state");
        }

        if(_remoteLinks.remove(remoteId))
        {
            _duty.linkEndRemove(remoteId);
            return;
        }

        if(_remoteLinks.endRemove(remoteId))
        {
            return;
        }

        throw link::source::Fail("bad remote link id marked to delete");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void LocalEdge::oppositeLinkEndRemove(link::LocalId localId)
    {
        if(apil::State::work != _state)
        {
            throw link::source::Fail("bad state");
        }

        _localLinks.endRemove(localId);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void LocalEdge::oppositeLinkEndRemove(link::RemoteId remoteId)
    {
        if(apil::State::work != _state)
        {
            throw link::source::Fail("bad state");
        }

        _remoteLinks.endRemove(remoteId);
    }
}
