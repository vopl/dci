/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "utils.hpp"

#ifdef _WIN32
#   include <sysinfoapi.h>
#   include <processthreadsapi.h>
#   include <error.h>
#   include <errhandlingapi.h>
#   include <processenv.h>
#   include <fileapi.h>
#   include <handleapi.h>
#   include <winioctl.h>
#   include <ioapiset.h>
#   include <iptypes.h>
#   include <iphlpapi.h>
#else
#   include <sys/utsname.h>
#   include <pwd.h>
#endif

namespace dci::module::ppn::node::utils
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string mkRandomName(size_t chars)
    {
        std::string res;
        res.resize(chars);

        crypto::rnd::generate(res.data(), chars);
        for(char& c : res)
        {
            c = (unsigned(c)%24) + unsigned('a');
        }

        return res;
    }

    namespace
    {
        const std::map<String, std::function<void(const config::ptree& config, crypto::Blake2b& accumuler)>> keyMaterialFetchers =
        {
            {
                "memInfo", [](const config::ptree&, crypto::Blake2b& accumuler)
                {
#ifdef _WIN32
                    MEMORYSTATUSEX mem{};
                    mem.dwLength = sizeof(mem);
                    if(!GlobalMemoryStatusEx(&mem))
                    {
                        throw api::Error("GlobalMemoryStatusEx failed: "+dci::utils::win32::error::last().message());
                    }
                    accumuler.add(mem.ullTotalPhys);
#else
                    std::ifstream in{"/proc/meminfo"};
                    if(!in)
                    {
                        throw api::Error("/proc/meminfo open failed: "+std::error_code{errno, std::generic_category()}.message());
                    }
                    std::string line;
                    while(std::getline(in, line))
                    {
                        if(line.starts_with("MemTotal:"))
                        {
                           accumuler.add(line);
                           break;
                        }
                    }
                    if(line.empty())
                    {
                        throw api::Error("/proc/meminfo malformed");
                    }
#endif
                }
            },
            {
                "cpuInfo", [](const config::ptree&, crypto::Blake2b& accumuler)
                {
#ifdef _WIN32
                    {
                        accumuler.add("IsProcessorFeaturePresent,0-63");
                        for(DWORD feature{}; feature<64; ++feature)
                        {
                            accumuler.add(IsProcessorFeaturePresent(feature));
                        }
                        accumuler.barrier();
                    }

                    {
                        DWORD length{};
                        if(!GetLogicalProcessorInformationEx(RelationAll, nullptr, &length) && ERROR_INSUFFICIENT_BUFFER == GetLastError() && length)
                        {
                            std::vector<char> buffer;
                            buffer.resize(length);
                            if(!GetLogicalProcessorInformationEx(
                                   RelationAll,
                                   reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(&buffer[0]),
                                   &length))
                            {
                                throw api::Error("GetLogicalProcessorInformationEx failed: "+dci::utils::win32::error::last().message());
                            }
                            accumuler.add("GetLogicalProcessorInformationEx,All");
                            accumuler.add(buffer.data(), buffer.size());
                            accumuler.barrier();
                        }
                    }

                    {
                        SYSTEM_INFO buffer{};
                        GetNativeSystemInfo(&buffer);
                        accumuler.add("GetNativeSystemInfo");
                        accumuler.add(&buffer, sizeof(buffer));
                        accumuler.barrier();
                    }
#else
                    std::ifstream in{"/proc/cpuinfo"};
                    if(!in)
                    {
                        throw api::Error("/proc/cpuinfo open failed: "+std::error_code{errno, std::generic_category()}.message());
                    }
                    std::string line;
                    while(std::getline(in, line))
                    {
                        if(std::string::npos != line.find("cpu MHz")) continue;
                        accumuler.add(line);
                        accumuler.barrier();
                    }
#endif
                }
            },
            {
                "diskInfo", [](const config::ptree&, crypto::Blake2b& accumuler)
                {
#ifdef _WIN32
                    std::set<std::wstring> drives;
                    std::set<std::wstring> volumes;
                    std::set<std::wstring> pdrives;
                    for(LPCWCHAR var : {L"%SYSTEMROOT%", L"%WINDIR%", L"%PROGRAMFILES%"})
                    {
                        accumuler.add("var");
                        accumuler.add(var);
                        accumuler.barrier();

                        WCHAR buf[MAX_PATH+1];
                        DWORD dw = ExpandEnvironmentStringsW(var, buf, MAX_PATH+1);
                        if(!dw)
                        {
                            throw api::Error("ExpandEnvironmentStringsA failed: "+dci::utils::win32::error::last().message());
                        }
                        std::wstring drive = std::filesystem::path{buf}.root_path();
                        if(!drives.insert(drive).second)
                        {
                            continue;
                        }
                        accumuler.add("drive");
                        accumuler.add(drive);
                        accumuler.barrier();

                        if(!GetVolumeNameForVolumeMountPointW(drive.c_str(), buf, MAX_PATH+1))
                        {
                            throw api::Error("GetVolumeNameForVolumeMountPointW failed: "+dci::utils::win32::error::last().message());
                        }
                        std::wstring volume = buf;
                        if(!volume.empty())
                        {
                            volume.pop_back();
                        }
                        if(!volumes.insert(volume).second)
                        {
                           continue;
                        }

                        DWORD serial{};
                        DWORD maximumComponentLength;
                        DWORD filesystemFlags;
                        WCHAR filesystemName[128];
                        if(!GetVolumeInformationW(
                               drive.c_str(),
                               buf, MAX_PATH+1, // label
                               &serial,
                               &maximumComponentLength,
                               &filesystemFlags,
                               filesystemName,
                               128))
                        {
                            throw api::Error("GetVolumeInformationW failed: "+dci::utils::win32::error::last().message());
                        }

                        accumuler.add("volume");
                        accumuler.add(volume);
                        accumuler.barrier();
                        accumuler.add(&buf[0]);
                        accumuler.barrier();
                        accumuler.add(serial);
                        accumuler.barrier();
                        accumuler.add(maximumComponentLength);
                        accumuler.barrier();
                        accumuler.add(filesystemFlags);
                        accumuler.barrier();
                        accumuler.add(&filesystemName[0]);
                        accumuler.barrier();

                        HANDLE hVolume = CreateFileW(volume.c_str(),
                                                     0,
                                                     FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                                     OPEN_EXISTING, 0, NULL);

                        if(INVALID_HANDLE_VALUE == hVolume)
                        {
                            throw api::Error("CreateFileW for "+dci::utils::str::wideConvert(volume)+" failed: "+dci::utils::win32::error::last().message());
                        }

                        char bigBuf[32768];
                        DWORD cbBytesReturned=0;
                        if(!DeviceIoControl(
                               hVolume,
                               IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
                               NULL,
                               0,
                               &bigBuf,
                               sizeof(bigBuf),
                               &cbBytesReturned,
                               NULL))
                        {
                            CloseHandle(hVolume);
                            throw api::Error("DeviceIoControl for "+dci::utils::str::wideConvert(volume)+" failed: "+dci::utils::win32::error::last().message());
                        }
                        CloseHandle(hVolume);

                        VOLUME_DISK_EXTENTS *pvde = (VOLUME_DISK_EXTENTS *)bigBuf;
                        if(!pvde->NumberOfDiskExtents)
                        {
                            continue;
                        }

                        for(DWORD pdNumber=0; pdNumber<pvde->NumberOfDiskExtents; pdNumber++)
                        {
                            std::wstring pdrive{L"\\\\.\\PhysicalDrive" + std::to_wstring(pdNumber)};
                            if(!pdrives.insert(pdrive).second)
                            {
                                continue;
                            }

                            HANDLE hPhysicalDrive = CreateFileW(
                                pdrive.c_str(),
                                0,
                                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                OPEN_EXISTING,
                                0,
                                NULL);

                            if(INVALID_HANDLE_VALUE == hPhysicalDrive)
                            {
                                throw api::Error("CreateFileW for "+dci::utils::str::wideConvert(pdrive)+" failed: "+dci::utils::win32::error::last().message());
                            }

                            STORAGE_PROPERTY_QUERY query;
                            DWORD cbBytesReturned = 0;
                            char buffer [10000];

                            memset ((void *) & query, 0, sizeof (query));
                            query.PropertyId = StorageDeviceProperty;
                            query.QueryType = PropertyStandardQuery;

                            memset (buffer, 0, sizeof (buffer));

                            if(!DeviceIoControl(hPhysicalDrive,
                                IOCTL_STORAGE_QUERY_PROPERTY,
                                &query,
                                sizeof(query),
                                &buffer,
                                sizeof(buffer),
                                &cbBytesReturned,
                                NULL))
                            {
                                CloseHandle(hPhysicalDrive);
                                throw api::Error("DeviceIoControl for "+dci::utils::str::wideConvert(pdrive)+" failed: "+dci::utils::win32::error::last().message());
                            }
                            CloseHandle(hPhysicalDrive);
                            STORAGE_DEVICE_DESCRIPTOR *descrip = (STORAGE_DEVICE_DESCRIPTOR *)buffer;

                            accumuler.add("pdrive");
                            accumuler.add(pdrive);
                            accumuler.barrier();
                            accumuler.add(descrip->Version);
                            accumuler.barrier();
                            accumuler.add(descrip->Size);
                            accumuler.barrier();
                            accumuler.add(descrip->DeviceType);
                            accumuler.barrier();
                            accumuler.add(descrip->DeviceTypeModifier);
                            accumuler.barrier();
                            accumuler.add(descrip->RemovableMedia);
                            accumuler.barrier();
                            accumuler.add(descrip->CommandQueueing);
                            accumuler.barrier();
                            if(descrip->VendorIdOffset)
                            {
                                accumuler.add(&buffer[descrip->VendorIdOffset]);
                            }
                            accumuler.barrier();
                            if(descrip->ProductIdOffset)
                            {
                                accumuler.add(&buffer[descrip->ProductIdOffset]);
                            }
                            accumuler.barrier();

                            if(descrip->ProductRevisionOffset)
                            {
                                accumuler.add(&buffer[descrip->ProductRevisionOffset]);
                            }
                            accumuler.barrier();

                            if(descrip->SerialNumberOffset)
                            {
                                accumuler.add(&buffer[descrip->SerialNumberOffset]);
                            }
                            accumuler.barrier();

                            accumuler.add(descrip->BusType);
                            accumuler.barrier();

                            accumuler.add(&descrip->RawDeviceProperties[0], descrip->RawPropertiesLength);
                            accumuler.barrier();
                        }
                    }
#else
                    namespace fs = std::filesystem;
                    std::set<fs::directory_entry> des;

                    if(fs::exists("/dev/disk/by-id"))
                    {
                        for(const fs::directory_entry& de : fs::directory_iterator{"/dev/disk/by-id"})
                        {
                            des.insert(de);
                        }
                    }

                    if(fs::exists("/dev/disk/by-uuid"))
                    {
                        for(const fs::directory_entry& de : fs::directory_iterator{"/dev/disk/by-uuid"})
                        {
                            des.insert(de);
                        }
                    }

                    if(des.empty())
                    {
                        throw api::Error("/dev/disk/by-id and /dev/disk/by-uuid missing");
                    }

                    for(const fs::directory_entry& de : des)
                    {
                        accumuler.add(de.path().filename().string());
                        accumuler.barrier();
                    }
#endif
                }
            },
            {
                "netMacAddress", [](const config::ptree&, crypto::Blake2b& accumuler)
                {
#ifdef _WIN32
                    ULONG outBufLen{};
                    GetAdaptersInfo(nullptr, &outBufLen);
                    std::vector<char> buf;
                    buf.resize(outBufLen);
                    IP_ADAPTER_INFO* pAdapterInfos = (IP_ADAPTER_INFO*)&buf[0];
                    if(GetAdaptersInfo(pAdapterInfos, &outBufLen))
                    {
                        throw api::Error("GetAdaptersInfo failed: "+dci::utils::win32::error::last().message());
                    }

                    while(pAdapterInfos)
                    {
                        accumuler.add(&pAdapterInfos->Address[0], pAdapterInfos->AddressLength);
                        accumuler.barrier();
                        pAdapterInfos = pAdapterInfos->Next;
                    }
#else
                    namespace fs = std::filesystem;
                    std::set<fs::directory_entry> des;
                    for(const fs::directory_entry& de : fs::directory_iterator{"/sys/class/net"})
                    {
                        if(fs::exists(de.path()/"device"))
                        {
                            des.insert(de);
                        }
                    }

                    for(const fs::directory_entry& de : des)
                    {
                        accumuler.add(de.path().filename().string());
                        accumuler.barrier();

                        std::ifstream in{de.path()/"address"};
                        if(!in)
                        {
                            throw api::Error((de.path()/"address").string() + " open failed: "+std::error_code{errno, std::generic_category()}.message());
                        }

                        bool someAdded{};
                        while(in)
                        {
                            std::string address;
                            in >> address;
                            if(!address.empty())
                            {
                                accumuler.add(address);
                                accumuler.barrier();
                                someAdded = true;
                            }
                        }
                        if(!someAdded)
                        {
                            throw api::Error((de.path()/"address").string() + " unexpected empty");
                        }
                    }
#endif
                }
            },
            {
                "osInfo", [](const config::ptree&, crypto::Blake2b& accumuler)
                {
#ifdef _WIN32
                    OSVERSIONINFOEXW buf{};
                    buf.dwOSVersionInfoSize = sizeof(buf);

                    if(!GetVersionExW((LPOSVERSIONINFOW)&buf))
                    {
                        throw api::Error("GetVersionExW failed: "+dci::utils::win32::error::last().message());
                    }
                    accumuler.add(&buf, sizeof(buf));

#else
                    utsname v;
                    if(uname(&v))
                    {
                        throw api::Error("uname failed: "+std::error_code{errno, std::generic_category()}.message());
                    }

                    accumuler.add("sysname");
                    accumuler.add(v.sysname);
                    accumuler.barrier();

                    accumuler.add("nodename");
                    accumuler.add(v.nodename);
                    accumuler.barrier();

                    accumuler.add("release");
                    accumuler.add(v.release);
                    accumuler.barrier();

                    accumuler.add("version");
                    accumuler.add(v.version);
                    accumuler.barrier();

                    accumuler.add("machine");
                    accumuler.add(v.machine);
                    accumuler.barrier();

                    std::ifstream in{"/proc/cmdline"};
                    if(!in)
                    {
                        throw api::Error("/proc/cmdline open failed: "+std::error_code{errno, std::generic_category()}.message());
                    }
                    std::string line;
                    while(std::getline(in, line))
                    {
                        accumuler.add("kernel cmdline");
                        accumuler.add(line);
                        accumuler.barrier();
                    }
#endif
                }
            },
            {
                "appPath", [](const config::ptree&, crypto::Blake2b& accumuler)
                {
#ifdef _WIN32
                    WCHAR path[2048]{};
                    DWORD dw = GetModuleFileNameW(nullptr, path, 2048);
                    if(!dw)
                    {
                        throw api::Error("GetModuleFileNameW failed: "+dci::utils::win32::error::last().message());
                    }
                    accumuler.add(&path, dw*sizeof(WCHAR));
#else
                    char path[PATH_MAX+1]{};
                    if(0 > readlink("/proc/self/exe", path, PATH_MAX))
                    {
                        throw api::Error("readlink /proc/self/exe failed: "+std::error_code{errno, std::generic_category()}.message());
                    }
                    accumuler.add(path);
#endif
                }
            },
            {
                "appPid", [](const config::ptree&, crypto::Blake2b& accumuler)
                {
#ifdef _WIN32
                    accumuler.add(GetCurrentProcessId());
#else
                    accumuler.add(getpid());
#endif
                }
            },
            {
                "domainname", [](const config::ptree&, crypto::Blake2b& accumuler)
                {
#ifdef _WIN32
                    WCHAR buf[256];
                    DWORD len = 256;
                    if(!GetComputerNameExW(ComputerNameDnsDomain, buf, &len))
                    {
                        throw api::Error("GetComputerNameExW ComputerNameDnsDomain failed: "+dci::utils::win32::error::last().message());
                    }
                    accumuler.add(&buf, len*sizeof(WCHAR));
#else
                    char name[256] = {0};
                    if(getdomainname(name, sizeof(name)-1))
                    {
                        throw api::Error("getdomainname failed: "+std::error_code{errno, std::generic_category()}.message());
                    }
                    accumuler.add(name);
#endif
                }
            },
            {
                "hostname", [](const config::ptree&, crypto::Blake2b& accumuler)
                {
   #ifdef _WIN32
                    WCHAR buf[256];
                    DWORD len = 256;
                    if(!GetComputerNameExW(ComputerNameDnsHostname, buf, &len))
                    {
                        throw api::Error("GetComputerNameExW ComputerNameDnsHostname failed: "+dci::utils::win32::error::last().message());
                    }
                    accumuler.add(&buf, len*sizeof(WCHAR));
   #else
                    char name[256] = {0};
                    if(gethostname(name, sizeof(name)-1))
                    {
                        throw api::Error("gethostname failed: "+std::error_code{errno, std::generic_category()}.message());
                    }
                    accumuler.add(name);
   #endif
                }
            },
            {
                "username", [](const config::ptree&, crypto::Blake2b& accumuler)
                {
#ifdef _WIN32
                    WCHAR buf[256];
                    DWORD len = 256;
                    if(!GetUserNameW(buf, &len))
                    {
                        throw api::Error("GetUserNameW failed: "+dci::utils::win32::error::last().message());
                    }
                    accumuler.add(&buf, len*sizeof(WCHAR));
#else
                    {
                        const char* name = getenv("USER");
                        if(name)
                        {
                            accumuler.add("env USER");
                            accumuler.add(name);
                            accumuler.barrier();
                            return;
                        }
                    }

                    errno = 0;
                    std::error_code ec_getlogin;
                    {
                        const char* name = getlogin();
                        if(name)
                        {
                            accumuler.add("getlogin");
                            accumuler.add(name);
                            accumuler.barrier();
                            return;
                        }
                        ec_getlogin = std::error_code{errno, std::generic_category()};
                    }

                    errno = 0;
                    std::error_code ec_getpwuid;
                    passwd* pw = getpwuid(geteuid());
                    if(pw)
                    {
                        accumuler.add("passwd pw_name");
                        accumuler.add(pw->pw_name);
                        accumuler.barrier();
                        return;
                    }
                    ec_getpwuid = std::error_code{errno, std::generic_category()};

                    throw api::Error("getlogin failed: "+ec_getlogin.message()+", getpwuid failed: "+ec_getpwuid.message());
#endif
                }
            },
            {
                "random", [](const config::ptree&, crypto::Blake2b& accumuler)
                {
                    char buf[256];
                    if(!crypto::rnd::generate(buf, sizeof(buf)))
                    {
                        throw api::Error("crypto::rnd failed");
                    }
                    accumuler.add(buf, sizeof(buf));
                }
            },
            {
                "constant", [](const config::ptree& config, crypto::Blake2b& accumuler)
                {
                    accumuler.add(config.get_value(String{}));
                }
            },
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    api::link::Key parseKey(const config::ptree& config)
    {
        api::link::Key res;

        crypto::Blake2b accumuler{res.size()};

        auto tryOne = [&](const String& kind, const config::ptree& conf = config::ptree{})
        {
            auto iter = keyMaterialFetchers.find(kind);
            if(keyMaterialFetchers.end() != iter)
            {
                accumuler.add(kind);
                iter->second(conf, accumuler);
                accumuler.barrier();
            }
            else
            {
                throw api::Error("ppn node: bad key material kind: "+kind);
            }
        };

        {
            String kind = config.get_value(String{});
            if(kind.empty() || "auto" == kind)
            {
                tryOne("memInfo");
                tryOne("cpuInfo");
                tryOne("diskInfo");
                tryOne("netMacAddress");
                tryOne("osInfo");

                tryOne("appPath");

                tryOne("domainname");
                tryOne("hostname");
                tryOne("username");

                tryOne("constant", config::ptree{String{"auto"}});
            }
            else
            {
                tryOne(kind);
            }
        }

        for(const auto&[kind, child] : config)
        {
            tryOne(kind, child);
        }

        dbgAssert(accumuler.digestSize() == res.size());
        accumuler.finish(res.data());
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint16 parseUint16(const String& param)
    {
        return static_cast<uint16>(std::stoull(param));
    }
}
