/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/test.hpp>
#include <dci/host.hpp>
#include "stiac.hpp"

using namespace dci;
using namespace dci::host;
using namespace dci::cmt;
using namespace dci::idl::gen::stiac;

namespace utils
{
    struct Bundle
    {
        Protocol<> _p1;
        LocalEdge<> _l1;
        RemoteEdge<> _r1;

        protocol::Requirements _inputRequirements = protocol::Requirements::null;
        protocol::Requirements _outputRequirements = protocol::Requirements::null;


        Protocol<> _p2;
        LocalEdge<> _l2;
        RemoteEdge<> _r2;

        sbs::Owner _session;


        Bundle(bool doInit = true, bool doStart = true)
        {
            if(doInit)
            {
                this->init();

                if(doStart)
                {
                    this->start();
                }
            }
        }

        void init()
        {
            _p1 = _manager->createService<Protocol<>>().value();
            _l1.init();
            _r1.init();
            _p1->setRemoteEdge(_r1.opposite());
            _p1->setLocalEdge(_l1.opposite());
            _p1->setRequirements(_inputRequirements, protocol::Requirements::null, _outputRequirements);

            _p2 = _manager->createService<Protocol<>>().value();
            _l2.init();
            _r2.init();
            _p2->setRemoteEdge(_r2.opposite());
            _p2->setLocalEdge(_l2.opposite());
            _p2->setRequirements(_outputRequirements, protocol::Requirements::null, _inputRequirements);


            _r1->output() += _session * [this](Bytes&& data)
            {
                _r2->input(std::move(data));
            };

            _r2->output() += _session * [this](Bytes&& data)
            {
                _r1->input(std::move(data));
            };
        }

        void start()
        {
            _p1->start();
            _p2->start();
        }

    private:
        Manager* _manager = testManager();
    };
}
