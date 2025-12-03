// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/integration/info.hpp>
#include <string>
#include <chrono>

namespace dci::integration::info
{
#define CAT0(x,y) x##y
#define CAT(x,y) CAT0(x,y)

    using namespace std::literals::string_view_literals;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string_view srcBranch()
    {
#if defined(DCI_SRC_BRANCH)
        return CAT(DCI_SRC_BRANCH, sv);
#else
        return {};
#endif
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string_view srcRevision()
    {
#if defined(DCI_SRC_REVISION)
        return CAT(DCI_SRC_REVISION, sv);
#else
        return {};
#endif
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::uint64_t srcMoment()
    {
#if defined(DCI_SRC_MOMENT)
        return DCI_SRC_MOMENT;
#else
        return {};
#endif
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string_view platformOs()
    {
#if defined(DCI_PLATFORM_OS)
        return CAT(DCI_PLATFORM_OS, sv);
#else
        return {};
#endif
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string_view platformArch()
    {
#if defined(DCI_PLATFORM_ARCH)
        return CAT(DCI_PLATFORM_ARCH, sv);
#else
        return {};
#endif
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string_view compiler()
    {
#if defined(DCI_COMPILER)
        return CAT(DCI_COMPILER, sv);
#else
        return {};
#endif
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string_view compilerVersion()
    {
#if defined(DCI_COMPILER_VERSION)
        return CAT(DCI_COMPILER_VERSION, sv);
#else
        return {};
#endif
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string_view compilerOptimization()
    {
#if defined(DCI_COMPILER_OPTIMIZATION)
        return CAT(DCI_COMPILER_OPTIMIZATION, sv);
#else
        return {};
#endif
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string_view vendor()
    {
#if defined(DCI_VENDOR)
        return CAT(DCI_VENDOR, sv);
#else
        return {};
#endif
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string_view API_DCI_INTEGRATION version()
    {
        static std::string res = version(srcBranch(), srcRevision(), srcMoment());
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string API_DCI_INTEGRATION version(std::string_view srcBranch, std::string_view srcRevision, std::uint64_t srcMoment)
    {
        return std::format("{:%F}-{}-{}",
            std::chrono::sys_time{std::chrono::duration<std::uint64_t>{srcMoment}},
            srcBranch,
            srcRevision.substr(0, 7));
    }
}
