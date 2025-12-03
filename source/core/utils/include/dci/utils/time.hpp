// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include <string>
#include <chrono>

namespace dci::utils::time
{
    API_DCI_UTILS const std::chrono::time_zone* currentZone();

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    API_DCI_UTILS double str2Moment(std::string_view str, const std::chrono::time_zone* targetZone = nullptr);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    API_DCI_UTILS void moment2Datetime6Tz(double epochSeconds, std::string& dst, const std::chrono::time_zone* targetZone = nullptr);
    API_DCI_UTILS void moment2Datetime3Tz(double epochSeconds, std::string& dst, const std::chrono::time_zone* targetZone = nullptr);
    API_DCI_UTILS void moment2DatetimeTz(double epochSeconds, std::string& dst, const std::chrono::time_zone* targetZone = nullptr);

    API_DCI_UTILS void moment2Datetime6(double epochSeconds, std::string& dst, const std::chrono::time_zone* targetZone = nullptr);
    API_DCI_UTILS void moment2Datetime3(double epochSeconds, std::string& dst, const std::chrono::time_zone* targetZone = nullptr);
    API_DCI_UTILS void moment2Datetime(double epochSeconds, std::string& dst, const std::chrono::time_zone* targetZone = nullptr);

    API_DCI_UTILS void moment2Date(double epochSeconds, std::string& dst, const std::chrono::time_zone* targetZone = nullptr);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    API_DCI_UTILS std::string moment2Datetime6Tz(double epochSeconds, const std::chrono::time_zone* targetZone = nullptr);
    API_DCI_UTILS std::string moment2Datetime3Tz(double epochSeconds, const std::chrono::time_zone* targetZone = nullptr);
    API_DCI_UTILS std::string moment2DatetimeTz(double epochSeconds, const std::chrono::time_zone* targetZone = nullptr);

    API_DCI_UTILS std::string moment2Datetime6(double epochSeconds, const std::chrono::time_zone* targetZone = nullptr);
    API_DCI_UTILS std::string moment2Datetime3(double epochSeconds, const std::chrono::time_zone* targetZone = nullptr);
    API_DCI_UTILS std::string moment2Datetime(double epochSeconds, const std::chrono::time_zone* targetZone = nullptr);

    API_DCI_UTILS std::string moment2Date(double epochSeconds, const std::chrono::time_zone* targetZone = nullptr);
}
