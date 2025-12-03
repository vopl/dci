// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/exception.hpp>
#include <stdexcept>
#include <string>

namespace dci::host
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Exception
        : public dci::exception::Skeleton<Exception, dci::Exception>
    {
    public:
        using dci::exception::Skeleton<Exception, dci::Exception>::Skeleton;

    public:
        static constexpr Eid _eid {0xc1,0x44,0xa6,0x9b,0xf8,0xb3,0x44,0xe0,0xab,0x1d,0x52,0xb4,0xfd,0x69,0xb1,0x81};
    };

    namespace exception
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        class TestFail
            : public dci::exception::Skeleton<TestFail, Exception>
        {
        public:
            using dci::exception::Skeleton<TestFail, Exception>::Skeleton;

        public:
            static constexpr Eid _eid {0xe9,0x5a,0x05,0x98,0x25,0xca,0x4f,0x52,0x8a,0xdc,0x94,0x50,0x2e,0x7d,0x87,0x70};
        };

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        class RunFail
            : public dci::exception::Skeleton<RunFail, Exception>
        {
        public:
            using dci::exception::Skeleton<RunFail, Exception>::Skeleton;

        public:
            static constexpr Eid _eid {0x31,0xfa,0xf5,0x63,0xfb,0x99,0x42,0xe6,0xb3,0x5d,0xa6,0x19,0xbe,0xdd,0x79,0x9c};
        };

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        class DaemonRunFail
            : public dci::exception::Skeleton<DaemonRunFail, Exception>
        {
        public:
            using dci::exception::Skeleton<DaemonRunFail, Exception>::Skeleton;

        public:
            static constexpr Eid _eid {0x63,0x30,0xb5,0x02,0x8d,0xc2,0x4b,0xf6,0xa2,0x7f,0xab,0xfd,0x88,0xd6,0x31,0xaf};
        };

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        class DaemonGetFail
            : public dci::exception::Skeleton<DaemonGetFail, Exception>
        {
        public:
            using dci::exception::Skeleton<DaemonGetFail, Exception>::Skeleton;

        public:
            static constexpr Eid _eid {0xbc,0xb1,0x0e,0x26,0x02,0x56,0x4a,0x01,0xac,0xc4,0x17,0xac,0x42,0x89,0xec,0xf4};
        };

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        class StopFail
            : public dci::exception::Skeleton<StopFail, Exception>
        {
        public:
            using dci::exception::Skeleton<StopFail, Exception>::Skeleton;

        public:
            static constexpr Eid _eid {0xcc,0xc0,0x57,0x46,0x1a,0xf2,0x43,0x03,0x9d,0xaa,0xd6,0x22,0x0c,0x04,0x45,0x9a};
        };

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        class UnableToCreateService
            : public dci::exception::Skeleton<UnableToCreateService, Exception>
        {
        public:
            using dci::exception::Skeleton<UnableToCreateService, Exception>::Skeleton;

        public:
            static constexpr Eid _eid {0x2d,0x27,0xd8,0x6b,0x82,0x8c,0x42,0x08,0x9d,0xb2,0x7b,0xef,0x3b,0x13,0x78,0xb5};
        };
    }
}
