/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "pumper.hpp"
#include "side.hpp"

namespace dci::module::ppn::transport::inproc::channelBridge
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Pumper::Pumper()
    {
        cmt::spawn() += _workerOwner * [this]
        {
            worker();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Pumper::~Pumper()
    {
        dbgAssert(_wants.empty());
        _workerOwner.stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Pumper::want(Side* s)
    {
        _wants.insert(s);
        _workerWaker.raise();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Pumper::unwant(Side* s)
    {
        _wants.erase(s);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Pumper::worker()
    {
        for(;;)
        {
            while(!_wants.empty())
            {
                Side* s = *_wants.begin();
                _wants.erase(_wants.begin());
                s->pump();
            }

            try
            {
                _workerWaker.wait();
            }
            catch(const cmt::task::Stop&)
            {
                dbgAssert(_wants.empty());

                while(!_wants.empty())
                {
                    Side* s = *_wants.begin();
                    _wants.erase(_wants.begin());

                    s->close();
                    s->pump();
                }

                break;
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    PumperPtr g_pumperPtr {};
}
