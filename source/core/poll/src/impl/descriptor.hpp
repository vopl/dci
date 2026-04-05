/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/cmt/task/owner.hpp>
#include <dci/cmt/raisable.hpp>
#include <dci/sbs/wire.hpp>
#include <dci/poll/descriptor/native.hpp>
#include <dci/poll/descriptor/readyStateFlags.hpp>
#include <dci/utils/intrusiveDlist.hpp>
#include <system_error>
#include <memory>

namespace dci::poll::impl
{
    class Polling;

    class Descriptor final
        : public utils::IntrusiveDlistElement<Descriptor>
    {
        Descriptor(const Descriptor&) = delete;
        void operator=(const Descriptor&) = delete;

    public:
        using Native = descriptor::Native;
        using ReadyStateFlags = descriptor::ReadyStateFlags;

    public:
        Descriptor(Native native, cmt::task::Owner* readyOwner, cmt::Raisable* raisable);
        ~Descriptor();

        sbs::Signal<void, Native /*native*/, ReadyStateFlags /*readyState*/> ready();
        void emitReadyIfNeed();
        void emitReady();

        void setReadyOwner(cmt::task::Owner* readyOwner);
        void resetReadyOwner();

        void setRaisable(cmt::Raisable* raisable);
        void resetRaisable();

        bool valid() const;
        std::error_code error();

        Native native() const;

        std::error_code close(bool withUninstall = true);

        std::error_code attach(Native native);
        std::error_code detach();

        ReadyStateFlags readyState() const;
        void resetReadyState(ReadyStateFlags flags);

    public:
        void setReadyState(ReadyStateFlags flags);//from polling engine

    private:
        std::error_code install();
        std::error_code uninstall();

    private:
        Native                                      _native;
        ReadyStateFlags                             _readyState{};

        struct Ready
        {
            Descriptor*                                 _owner;
            bool                                        _inProgress{};
            sbs::Wire<void, Native, ReadyStateFlags>    _wire;

            Ready(Descriptor* owner) : _owner{owner} {}
        };
        using ReadyPtr = std::shared_ptr<Ready>;
        ReadyPtr                                    _ready{std::make_shared<Ready>(this)};

        cmt::task::Owner                            _localReadyOwner{};
        cmt::task::Owner*                           _readyOwner{&_localReadyOwner};
        cmt::Raisable*                              _raisable{};

        long long stub[5];
    };
}
