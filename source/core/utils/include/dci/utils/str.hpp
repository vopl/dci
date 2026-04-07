/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "api.hpp"
#include <string>

namespace dci::utils::str
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    API_DCI_UTILS void wideConvert(const std::wstring_view& src, std::string& dst);
    API_DCI_UTILS void wideConvert(const std::string_view& src, std::wstring& dst);
    API_DCI_UTILS std::string wideConvert(const std::wstring_view& src);
    API_DCI_UTILS std::wstring wideConvert(const std::string_view& src);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    API_DCI_UTILS void wideConvert(const wchar_t* src, std::string& dst);
    API_DCI_UTILS void wideConvert(const char* src, std::wstring& dst);
    API_DCI_UTILS std::string wideConvert(const wchar_t* src);
    API_DCI_UTILS std::wstring wideConvert(const char* src);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    API_DCI_UTILS void wideConvert(const std::wstring& src, std::string& dst);
    API_DCI_UTILS void wideConvert(const std::string& src, std::wstring& dst);
    API_DCI_UTILS std::string wideConvert(const std::wstring& src);
    API_DCI_UTILS std::wstring wideConvert(const std::string& src);
}
