// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "duty.hpp"
#include "../localEdge.hpp"

namespace dci::module::stiac::localEdge
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Duty::Duty(LocalEdge* le)
        : _le{le}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Duty::~Duty()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<None> Duty::putInterface(Interface&& interface, ILid targetIlid)
    {
        return call2Bin<cmt::Future<None>>(
                    link::MethodId(3),
                    link::serialization::NonMdInterface{std::move(interface), targetIlid});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Duty::optimisticPutInterface(Interface&& interface, ILid targetIlid)
    {
        return call2Bin<void>(
                    link::MethodId(4),
                    link::serialization::NonMdInterface{std::move(interface), targetIlid});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Duty::linkBeginRemove(link::LocalId localId)
    {
        return call2Bin<void>(
                    link::MethodId(5),
                    localId);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Duty::linkBeginRemove(link::RemoteId remoteId)
    {
        return call2Bin<void>(
                    link::MethodId(6),
                    remoteId);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Duty::linkEndRemove(link::LocalId localId)
    {
        return call2Bin<void>(
                    link::MethodId(7),
                    localId);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Duty::linkEndRemove(link::RemoteId remoteId)
    {
        return call2Bin<void>(
                    link::MethodId(8),
                    remoteId);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Duty::input(link::Source& source)
    {
        link::MethodId methodId;
        source >> methodId;

        switch(static_cast<uint32>(methodId))
        {
        case 3://put
            return bin2Call<cmt::Future<>, Interface>(source, [&](Interface&& interface)
            {
                return _le->oppositePutInterface(std::move(interface));
            });

        case 4://optimisticPut
            return bin2Call<void, Interface>(source, [&](Interface&& interface)
            {
                return _le->oppositeOptimisticPutInterface(std::move(interface));
            });

        case 5://linkBeginRemove local
            return bin2Call<void, link::RemoteId>(source, [&](link::RemoteId remoteId)
            {
                return _le->oppositeLinkBeginRemove(remoteId);
            });

        case 6://linkBeginRemove remote
            return bin2Call<void, link::LocalId>(source, [&](link::LocalId localId)
            {
                return _le->oppositeLinkBeginRemove(localId);
            });

        case 7://linkEndRemove local
            return bin2Call<void, link::RemoteId>(source, [&](link::RemoteId remoteId)
            {
                return _le->oppositeLinkEndRemove(remoteId);
            });

        case 8://linkEndRemove remote
            return bin2Call<void, link::LocalId>(source, [&](link::LocalId localId)
            {
                return _le->oppositeLinkEndRemove(localId);
            });

        default:
            {
                source.fail("malformed input");
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Duty::destroy()
    {
        dbgWarn("never here");
    }

}
