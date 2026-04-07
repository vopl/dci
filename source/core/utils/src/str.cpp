/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/utils/str.hpp>

#ifdef WIN32
#   include <cstring>
#   include <cwchar>
#   include <stringapiset.h>
#else
#   include <boost/locale.hpp>
#endif

namespace dci::utils::str
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void wideConvert(const std::wstring_view& src, std::string& dst)
    {
        if(src.empty())
        {
            dst.clear();
            return;
        }

#ifdef WIN32
        int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, src.data(), static_cast<int>(src.size()), nullptr, 0, nullptr, nullptr);
        if(sizeNeeded <= 0)
        {
            //throw "WideCharToMultiByte() failed";
            dst.clear();
            return;
        }

        dst.resize(sizeNeeded);
        WideCharToMultiByte(CP_UTF8, 0, src.data(), static_cast<int>(src.size()), dst.data(), sizeNeeded, nullptr, nullptr);
#else
        dst = boost::locale::conv::utf_to_utf<char>(src.begin(), src.end());
#endif
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void wideConvert(const std::string_view& src, std::wstring& dst)
    {
        if(src.empty())
        {
            dst.clear();
            return;
        }

#ifdef WIN32
        int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, src.data(), static_cast<int>(src.size()), nullptr, 0);
        if(sizeNeeded <= 0)
        {
            //throw "MultiByteToWideChar() failed";
            dst.clear();
            return;
        }

        dst.resize(sizeNeeded);
        MultiByteToWideChar(CP_UTF8, 0, src.data(), static_cast<int>(src.size()), dst.data(), sizeNeeded);
#else
        dst = boost::locale::conv::utf_to_utf<wchar_t>(src.begin(), src.end());
#endif
        return;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string wideConvert(const std::wstring_view& src)
    {
        std::string res;
        wideConvert(src, res);
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::wstring wideConvert(const std::string_view& src)
    {
        std::wstring res;
        wideConvert(src, res);
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void wideConvert(const wchar_t* src, std::string& dst)
    {
        return wideConvert(std::wstring_view{src, src + std::wcslen(src)}, dst);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void wideConvert(const char* src, std::wstring& dst)
    {
        return wideConvert(std::string_view{src, src + std::strlen(src)}, dst);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string wideConvert(const wchar_t* src)
    {
        return wideConvert(std::wstring_view{src, src + std::wcslen(src)});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::wstring wideConvert(const char* src)
    {
        return wideConvert(std::string_view{src, src + std::strlen(src)});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void wideConvert(const std::wstring& src, std::string& dst)
    {
        return wideConvert(std::wstring_view{src}, dst);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void wideConvert(const std::string& src, std::wstring& dst)
    {
        return wideConvert(std::string_view{src}, dst);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string wideConvert(const std::wstring& src)
    {
        return wideConvert(std::wstring_view{src});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::wstring wideConvert(const std::string& src)
    {
        return wideConvert(std::string_view{src});
    }
}
