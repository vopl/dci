/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <iostream>
#include <fstream>

#include <dci/logger.hpp>
#include <dci/host.hpp>
#include <dci/cmt.hpp>
#include <dci/utils/atScopeExit.hpp>
#include <dci/utils/b2h.hpp>
#include <dci/exception.hpp>
#include <dci/aup/instance/setup.hpp>
#include <dci/aup/instance/io.hpp>
#include <dci/aup/instance/notifiers.hpp>
#include <dci/poll/functions.hpp>
#include <dci/poll/awaker.hpp>
#include <dci/sbs.hpp>
#include <dci/mm.hpp>
#include <dci/integration/info.hpp>
#include <filesystem>
#include <boost/program_options.hpp>
#include <csignal>

#include <boost/stacktrace.hpp>

#if __has_include(<link.h>)
#   include <link.h>
#endif

#ifdef _WIN32
#   include <windows.h>
#   include <psapi.h>
#   include <dbghelp.h>
#   include <winnt.h>
#else
#   include <sys/types.h>
#   include <dirent.h>
#endif

namespace fs = std::filesystem;
namespace po = boost::program_options;

using namespace dci::host;
using namespace dci::cmt;

auto tryCatch(const std::string& name, auto&& f, auto&& back)
{
    try
    {
        return f();
    }
    catch(...)
    {
        LOGE(name+": "+dci::exception::toString(std::current_exception()));
        return back();
    }
}

namespace
{
    Manager *           manager {nullptr};
    bool                aupApplied {false};
    fs::path            executablePath;
    std::size_t         stopsCount {};
    bool                stopInitiated {};
    dci::poll::Awaker*  pollAwaker{};
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
static void dumpStacks()
{
    //TODO:  async-safe
    std::cout << "dumpStack to /tmp/dump2" << std::endl;
    std::ofstream out{"/tmp/dump2"};

    if(!out)
    {
        return;
    }

    out << "srcBranch           : " << dci::integration::info::srcBranch() << '\n';
    out << "srcRevision         : " << dci::integration::info::srcRevision() << '\n';
    out << "platformOs          : " << dci::integration::info::platformOs() << '\n';
    out << "platformArch        : " << dci::integration::info::platformArch() << '\n';
    out << "compiler            : " << dci::integration::info::compiler() << '\n';
    out << "compilerVersion     : " << dci::integration::info::compilerVersion() << '\n';
    out << "compilerOptimization: " << dci::integration::info::compilerOptimization() << '\n';
    out << "vendor              : " << dci::integration::info::vendor() << '\n';

#ifdef _WIN32
    out << "modules" << std::endl;
    std::vector<HMODULE> hMods;
    hMods.resize(32);
    DWORD cbNeeded = 0;
    HANDLE hProcess = GetCurrentProcess();
    while(EnumProcessModules(hProcess, hMods.data(), hMods.size()*sizeof(HMODULE), &cbNeeded) && cbNeeded > hMods.size()*sizeof(HMODULE))
    {
        hMods.resize(cbNeeded/sizeof(HMODULE));
    }

    for(DWORD i{}; i < (cbNeeded/sizeof(HMODULE)); i++)
    {
        fs::path modName;
        WCHAR szModName[MAX_PATH];

        if(GetModuleFileNameExW(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(WCHAR)))
        {
            modName = fs::path{szModName};
        }

        ///////////////////////////////////////////////
        out << reinterpret_cast<const void*>(hMods[i]) << " " << modName.string();

        bool idFetched = false;
        if(!idFetched)
        {
            const char *image = (const char *)hMods[i];
            IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)image;
            IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)&image[dosHeader->e_lfanew];
            for(WORD sectionIndex{}; sectionIndex < ntHeaders->FileHeader.NumberOfSections; ++sectionIndex)
            {
                IMAGE_SECTION_HEADER* section = (IMAGE_SECTION_HEADER*)&image[dosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS) + sectionIndex*sizeof(IMAGE_SECTION_HEADER)];
                if(section->Misc.VirtualSize < 48)
                {
                    continue;
                }

                if(strncmp((const char*)&section->Name[0], ".buildid", IMAGE_SIZEOF_SHORT_NAME))
                {
                    continue;
                }

                const dci::uint32 type = *(const dci::uint32*)&image[section->VirtualAddress + 28];
                if(0x53445352 != type)//RSDS
                {
                    continue;
                }

                const dci::byte* hash = (const dci::byte*)&image[section->VirtualAddress + 32];
                out << " gnu:";
                out << dci::utils::b2h(hash+0, 4, dci::utils::HexEndian::big);
                out << dci::utils::b2h(hash+4, 2, dci::utils::HexEndian::big);
                out << dci::utils::b2h(hash+6, 2, dci::utils::HexEndian::big);
                out << dci::utils::b2h(hash+8, 8, dci::utils::HexEndian::middle);
                idFetched = true;
                break;
            }
        }

        if(!idFetched)
        {
            SYMSRV_INDEX_INFOW info{};
            info.sizeofstruct = sizeof(info);
            if(SymSrvGetFileIndexInfoW(szModName, &info, 0))
            {
                out << " ms:";
                char buf[64];
                std::sprintf(buf, "%lx%lx", info.timestamp, info.size);
                out << buf;
                idFetched = true;
            }
        }
        out << std::endl;
    }
#else
    out << "shared objects" << std::endl;
    {
        dl_iterate_phdr([](struct dl_phdr_info *info, size_t /*size*/, void *data) -> int
        {
            ///////////////////////////////////////////////
            const char *soName = info->dlpi_name;
            char exe[PATH_MAX];
            if(!soName || !soName[0])
            {
                ssize_t ret = readlink("/proc/self/exe", exe, sizeof(exe)-1);
                if(ret >= 0)
                {
                    exe[ret] = 0;
                    soName = exe;
                }
            }

            ///////////////////////////////////////////////
            struct BuildIdNote
            {
                ElfW(Nhdr)  _nhdr;
                char        _name[4];
                uint8_t     _data[1];
            };
            BuildIdNote* buildIdNote{};

            for(std::size_t i{}; i < info->dlpi_phnum; ++i)
            {
                if(PT_NOTE != info->dlpi_phdr[i].p_type)
                {
                    continue;
                }

                BuildIdNote* note = (BuildIdNote*)(info->dlpi_addr + info->dlpi_phdr[i].p_vaddr);
                std::size_t len = info->dlpi_phdr[i].p_filesz;

                while(len >= sizeof(BuildIdNote))
                {
                    if (note->_nhdr.n_type == NT_GNU_BUILD_ID &&
                        note->_nhdr.n_descsz != 0 &&
                        note->_nhdr.n_namesz == 4 &&
                        memcmp(note->_name, "GNU", 4) == 0)
                    {
                        buildIdNote = note;
                        break;
                    }

                    auto align = [](std::size_t val, std::size_t align)
                    {
                        return (((val) + (align) - 1) & ~((align) - 1));
                    };

                    std::size_t offset = sizeof(ElfW(Nhdr)) +
                                    align(note->_nhdr.n_namesz, 4) +
                                    align(note->_nhdr.n_descsz, 4);
                    note = (BuildIdNote*)((char*)note + offset);
                    len -= offset;
                }
            }

            ///////////////////////////////////////////////
            std::ofstream& out = *static_cast<std::ofstream*>(data);
            out << reinterpret_cast<const void*>(info->dlpi_addr) << " " << soName;
            if(buildIdNote)
            {
                out << " " << dci::utils::b2h(buildIdNote->_data, buildIdNote->_nhdr.n_descsz, dci::utils::HexEndian::middle);
            }
            out << std::endl;
            return 0;
        }, &out);
    }
#endif

    dci::cmt::enumerateFibers([](dci::cmt::task::State state, void *data)
    {
        std::ofstream& out = *static_cast<std::ofstream*>(data);

        out << "ctx[";
        switch(state)
        {
        case dci::cmt::task::State::null            : out << "null"             ; break;
        case dci::cmt::task::State::ready4Stop      : out << "ready4Stop"       ; break;
        case dci::cmt::task::State::ready           : out << "ready"            ; break;
        case dci::cmt::task::State::readyLowPriority: out << "readyLowPriority" ; break;
        case dci::cmt::task::State::work            : out << "work"             ; break;
        case dci::cmt::task::State::hold            : out << "hold"             ; break;
        default: out << "unknown"; break;
        }
        out << "]";

        boost::stacktrace::stacktrace st{};

        for(const boost::stacktrace::frame& frame : st)
        {
            out << ' ' << frame.address();
        }
        out << '\n';

    }, &out);

    out.close();
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
static void signalHandler(int signum)
{
    switch(signum)
    {
    case SIGINT:
    case SIGTERM:
#ifdef SIGQUIT
    case SIGQUIT:
#endif
#ifdef SIGTSTP
    case SIGTSTP:
#endif
        if(++stopsCount >= std::size_t{3} || !manager)
        {
            dumpStacks();
#ifdef _WIN32
            ::TerminateProcess(GetCurrentProcess(), EXIT_FAILURE);
#else
            _exit(EXIT_FAILURE);
#endif
        }

        if(pollAwaker)
        {
            pollAwaker->wakeup();
        }
        break;

#ifdef SIGUSR1
    case SIGUSR1:
        dumpStacks();
        break;
#endif
    default:
        dumpStacks();
#ifdef _WIN32
        ::TerminateProcess(GetCurrentProcess(), EXIT_FAILURE);
#else
        _exit(EXIT_FAILURE);
#endif
    }
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
static int afterAupApplied(bool restart, std::vector<std::string> argv)
{
    if(!restart)
    {
        LOGI("exit 3 because of aup");
        return 3;
    }

    LOGI("restart because of aup");

    argv[0] = executablePath.string();

    std::vector<char*> c_argv;
    c_argv.reserve(argv.size()+1);

    for(std::string& arg : argv)
    {
        c_argv.push_back(arg.data());
    }
    c_argv.push_back(nullptr);

#ifndef _WIN32
    {
        std::deque<int> fds;
        {
            DIR* dir = ::opendir("/proc/self/fd");
            while(dirent* de = ::readdir(dir))
            {
                if(DT_LNK == de->d_type)
                {
                    if(int fd = ::atoi(de->d_name); 3 <= fd)
                    {
                        fds.emplace_back(fd);
                    }
                }
            }
            ::closedir(dir);
        }

        for(int fd : fds)
        {
            ::close(fd);
        }
    }
#endif

    if(execv(executablePath.string().c_str(), c_argv.data()))
    {
        LOGF("unable to restart: "<<strerror(errno));
    }

    return EXIT_FAILURE;
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
bool printOutput(const po::variables_map& vars, const std::string& content);

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
static std::chrono::system_clock::time_point timeProvider4Logging()
{
    return std::chrono::system_clock::now();
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
int main(int c_argc, char* c_argv[])
{
    //set current path near executable
    {
        std::error_code ec;
        executablePath = c_argv[0];
        executablePath = fs::canonical(executablePath, ec);
        if(ec)
        {
            LOGE("unable to determine current directory: "<<ec);
            return EXIT_FAILURE;
        }

        fs::path executableDir = executablePath.parent_path();
        fs::current_path(executableDir, ec);
        if(ec)
        {
            LOGE("unable to set current directory to "<<executableDir<<": "<<ec);
            return EXIT_FAILURE;
        }
    }

    //setup logging
    dci::logger::setTimeProvider(&timeProvider4Logging);

    //setup signals
    signal(SIGINT,  signalHandler);
    signal(SIGTERM, signalHandler);
#ifdef SIGQUIT
    signal(SIGQUIT, signalHandler);
#endif
#ifdef SIGTSTP
    signal(SIGTSTP, signalHandler);
#endif
#ifdef SIGUSR1
    signal(SIGUSR1, signalHandler);
#endif
    dci::mm::setupPanicHandler(signalHandler);

    //formalize args
    const std::vector<std::string> argv = [&]()
    {
        std::vector<std::string> res;
        res.reserve(static_cast<std::size_t>(c_argc));

        for(int i(0); i<c_argc; ++i)
        {
            res.emplace_back(c_argv[i]);
        }

        return res;
    }();

    ////////////////////////////////////////////////////////////////////////////////
    po::options_description desc("dci-host");
    desc.add_options()
            ("help", "produce help message")
            ("version", "print version info")
            (
                "genmanifest",
                po::value<std::string>(),
                "generate manifest file for module shared library"
            )
            (
                "outfile",
                po::value<std::string>(),
                "output file name for genmanifest"
            )
            (
                "module",
                po::value<std::vector<std::string>>()->multitoken()->default_value(std::vector<std::string>{""}, ""),
                "specify module(s) to run"
            )
            (
                "service",
                po::value<std::vector<std::string>>()->multitoken()->default_value(std::vector<std::string>{}, ""),
                "modules to run that contains the service(s) specified"
            )
            (
                "test",
                po::value<std::string>(),
                "stage for testing, one of noenv|mnone|mstart"
            )
            (
                "run",
                po::value<std::vector<std::string>>()->multitoken(),
                "run daemon"
            )
            (
                "runN",
                po::value<std::vector<std::string>>()->multitoken(),
                "run daemon multiple times"
            )
            (
                "aup",
                po::value<std::vector<std::string>>()->multitoken()->implicit_value({"@../etc/aup.conf"}, "@../etc/aup.conf"),
                "use automatic updates"
            )
            (
                "aup-reaction",
                po::value<std::string>()->default_value("restart"),
                "automatic update reaction, one of restart|exit"
            )
            ;

    ////////////////////////////////////////////////////////////////////////////////
    po::variables_map vars;
    po::parsed_options parsedOptions(&desc);

    tryCatch("commandline",
        [&]{
            parsedOptions = po::command_line_parser(argv).options(desc).allow_unregistered().run();
            po::store(parsedOptions, vars);
        },
        []{
            exit(EXIT_FAILURE);
        });
    po::notify(vars);

    ////////////////////////////////////////////////////////////////////////////////
    if(vars.empty() || vars.count("version"))
    {
        std::cout << dci::integration::info::version() << std::endl;
        return EXIT_SUCCESS;
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(vars.count("help"))
    {
        std::cout << desc << std::endl;
        return EXIT_SUCCESS;
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(vars.count("genmanifest"))
    {
        std::string content = tryCatch("genmanifest",
            [&]{
                return Manager::moduleManifest(vars["genmanifest"].as<std::string>()).toConf();
            },
            []{
                return std::string();
            });

        if(content.empty())
        {
            return EXIT_FAILURE;
        }

        return printOutput(vars, content) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    if(stopsCount)
    {
        LOGI("exit by request");
        return EXIT_SUCCESS;
    }

    ////////////////////////////////////////////////////////////////////////////////
    TestStage testStage = TestStage::null;
    if(vars.count("test"))
    {
        {
            auto s  = vars["test"].as<std::string>();

                 if("noenv"  == s) testStage = dci::host::TestStage::noenv;
            else if("mnone"  == s) testStage = dci::host::TestStage::mnone;
            else if("mstart" == s) testStage = dci::host::TestStage::mstart;
            else
            {
                LOGF("unrecognized test stage: "<<s);
                return EXIT_FAILURE;
            }
        }

        if(TestStage::noenv == testStage)
        {
            tryCatch("executeTest",
                [&]{
                    int res = Manager::executeTest(argv, testStage);
                    exit(res);
                },
                []{
                    exit(EXIT_FAILURE);
                });
        }
    }

    if(stopsCount)
    {
        LOGI("exit by request");
        return EXIT_SUCCESS;
    }

    ////////////////////////////////////////////////////////////////////////////////
    auto argAup = vars["aup"];
    bool aupReactionRestart{};
    {
        std::string aupReaction = vars["aup-reaction"].as<std::string>();
        if("restart" == aupReaction)
        {
            aupReactionRestart = true;
        }
        else if("exit" == aupReaction)
        {
            aupReactionRestart = false;
        }
        else
        {
            LOGF("unrecognized aup-reaction: "<<aupReaction);
            return EXIT_FAILURE;
        }
    }

    if(!argAup.empty() && TestStage::null == testStage)
    {
        tryCatch("setupAup",
            [&]{

                    dci::aup::instance::setup::start(argAup.as<std::vector<std::string>>());

                    auto updateIfNeed = []
                    {
                        if(!dci::aup::instance::setup::targetComplete())
                        {
                            return;
                        }

                        static dci::Set<dci::aup::Oid> targetMostReleases_previous;

                        dci::Set<dci::aup::Oid> targetMostReleases = dci::aup::instance::io::targetMostReleases();
                        if(targetMostReleases.empty())
                        {
                            return;
                        }

                        if(targetMostReleases == targetMostReleases_previous)
                        {
                            return;
                        }

                        try
                        {
                            LOGI("update target by aup");
                            dci::aup::instance::setup::updateTarget();

                            targetMostReleases_previous = targetMostReleases;
                        }
                        catch(...)
                        {
                            LOGW("failed: "<<dci::exception::toString(std::current_exception()));
                        }
                    };

                    dci::aup::instance::notifiers::onTargetTotallyComplete() += [=]
                    {
                        dci::aup::instance::setup::collectGarbage();
                        updateIfNeed();
                    };

                    dci::aup::instance::notifiers::onBufferTotallyComplete() += [=]
                    {
                        dci::aup::instance::setup::collectGarbage();
                    };

                    dci::aup::instance::notifiers::onTargetUpdated() += [&](dci::aup::applier::Result res)
                    {
                        if(dci::aup::applier::rFixedMissings & res ||
                           dci::aup::applier::rFixedChanges & res ||
                           dci::aup::applier::rFixedExtra & res)
                        {
                            LOGI("target is updated by aup");

                            aupApplied = true;
                            if(manager)
                            {
                                manager->stop();
                            }
                        }
                        else if(dci::aup::applier::rCorruptedCatalog & res ||
                                dci::aup::applier::rAmbiguousCatalog & res ||
                                dci::aup::applier::rIncompleteCatalog & res ||
                                dci::aup::applier::rIncompleteStorage & res)
                        {
                            LOGW("target is NOT updated");
                        }
                        else
                        {
                            LOGI("target is actual");
                        }

                    };

                    dci::aup::instance::setup::collectGarbage();
                    updateIfNeed();
            },
            []{
                exit(EXIT_FAILURE);
            });
    }

    if(stopsCount)
    {
        LOGI("exit by request");
        return EXIT_SUCCESS;
    }

    if(aupApplied)
    {
         return afterAupApplied(aupReactionRestart, argv);
    }

    auto fetchMultitokenArgs = [&](const std::string& name) -> std::vector<std::vector<std::string>>
    {
        std::vector<std::vector<std::string>> res;
        for(const po::basic_option<char>& o : parsedOptions.options)
        {
            if(!o.unregistered && -1==o.position_key && name==o.string_key)
            {
                res.push_back(o.value);
            }
        }

        return res;
    };

    int processResultCode = EXIT_SUCCESS;

    dci::sbs::Wire<> modulesStarted;

    ////////////////////////////////////////////////////////////////////////////////
    //if(vars.count("run") || vars.count("runN") || (TestStage::null != testStage && TestStage::noenv != testStage))
    {
        manager = new Manager;

        dci::utils::AtScopeExit se{[=]
        {
            delete std::exchange(manager, nullptr);
        }};

        dci::sbs::Owner testRunnerOwner;
        auto testRunner = [&]()
        {
            manager->runTest(argv, testStage).then() += [&](auto in)
            {
                if(in.resolvedException())
                {
                    LOGE("run test failed: "<<dci::exception::toString(in.exception()));
                    processResultCode = EXIT_FAILURE;
                }

                tryCatch("fetch test exit code", [&]
                {
                    processResultCode = in.value();
                }, [&]
                {
                    processResultCode = -1;
                });

                manager->stop();
            };
        };

        if(TestStage::mnone == testStage)
        {
            dci::poll::started() += testRunner;
        }

        if(TestStage::mstart == testStage)
        {
            modulesStarted.out() += testRunner;
        }

        for(const std::vector<std::string>& argv : fetchMultitokenArgs("run"))
        {
            if(1 > argv.size())
            {
                LOGE("run arg must specify daemon name");
                continue;
            }

            modulesStarted.out() += [=]
            {
                manager->runDaemon(argv).then() += [=](auto in)
                {
                    if(in.resolvedException())
                    {
                        LOGE("daemon run failed: "<<argv[0]<<", "<<dci::exception::toString(in.exception()));
                    }
                };
            };
        }

        for(const std::vector<std::string>& argv : fetchMultitokenArgs("runN"))
        {
            if(2 > argv.size())
            {
                LOGE("runN arg must specify daemons amount and daemon name");
                continue;
            }

            modulesStarted.out() += [=]
            {
                manager->runDaemons(argv).then() += [=](auto in)
                {
                    if(in.resolvedException())
                    {
                        LOGE("daemons run failed: "<<argv[0]<<" x "<<argv[1]<<", "<<dci::exception::toString(in.exception()));
                    }
                };
            };
        }

        dci::sbs::Owner pollAwakerOwner;
        dci::poll::Awaker pollAwakerInstance{false};
        pollAwaker = &pollAwakerInstance;
        dci::utils::AtScopeExit cleaner{[&]{pollAwaker = nullptr;}};

        pollAwaker->woken() += pollAwakerOwner * [&]
        {
            if(stopsCount)
            {
                if(!stopInitiated)
                {
                    stopInitiated = true;
                    LOGI("stop manager");
                    manager->stop();
                }
                else
                {
                    LOGI("await previous stopping");
                }
            }
        };

        dci::poll::started() += [&]
        {
            std::vector<std::string> modules = vars["module"].as<std::vector<std::string>>();
            std::set<std::string> modulesSet{std::make_move_iterator(modules.begin()), std::make_move_iterator(modules.end())};

            std::vector<std::string> services = vars["service"].as<std::vector<std::string>>();
            std::set<std::string> servicesSet{std::make_move_iterator(services.begin()), std::make_move_iterator(services.end())};

            if(manager->startModules(std::move(modulesSet), std::move(servicesSet)))
            {
                modulesStarted.in();
            }
            else
            {
                manager->stop();
            }
        };

        tryCatch("run",
            [&]{
                LOGI("manager run");
                manager->run();
                LOGI("manager done");
            },
            [&]{
                LOGI("manager failed");
                processResultCode = EXIT_FAILURE;
            });
    }

    if(stopsCount)
    {
        LOGI("exit by request");
        return processResultCode;
    }

    if(aupApplied && EXIT_SUCCESS==processResultCode)
    {
        return afterAupApplied(aupReactionRestart, argv);
    }

    return processResultCode;
}

bool printOutput(const po::variables_map& vars, const std::string& content)
{
    if(vars.count("outfile"))
    {
        std::ofstream out(vars["outfile"].as<std::string>());
        if(!out)
        {
            std::cerr<<strerror(errno)<<std::endl;
            return false;
        }

        out.write(content.data(), static_cast<std::streamsize>(content.size()));
        return true;
    }

    std::cout.write(content.data(), static_cast<std::streamsize>(content.size()));
    return true;
}
