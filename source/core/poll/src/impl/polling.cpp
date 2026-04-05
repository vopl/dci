/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "polling.hpp"

#include <dci/utils/dbg.hpp>
#include <dci/poll/descriptor.hpp>

namespace dci::poll::impl
{

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Polling::Polling()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Polling::~Polling()
    {
        _descriptors.flush([](Descriptor* d)
        {
            d->close(false);
            d->setReadyState(descriptor::rsf_error);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Polling::initialize()
    {
        return _engine.initialize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Polling::initialized() const
    {
        return _engine.initialized();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Polling::installDescriptor(Descriptor* d)
    {
        std::error_code ec = _engine.installDescriptor(d);

        if(ec)
        {
            return ec;
        }

        dbgAssert(!_descriptors.contains(d));
        _descriptors.push(d);

        return ec;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Polling::uninstallDescriptor(Descriptor* d)
    {
        std::error_code ec = _engine.uninstallDescriptor(d);

        if(d->emplaced())
            _descriptors.remove(d);

        return ec;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Polling::execute(clocking::Duration timeout)
    {
        return _engine.execute(timeout);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Polling::wakeup()
    {
        return _engine.wakeup();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Polling::deinitialize()
    {
        return _engine.deinitialize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Polling::hasPayload() const
    {
        return !_descriptors.empty();
    }
}
