/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/utils/time.hpp>
#include <dci/utils/str.hpp>
#include <string>
#include <string_view>
#include <chrono>
#include <format>
#include <spanstream>

#if _WIN32
#   include <unordered_map>
#   include <windows.h>
#endif

namespace dci::utils::time
{
    using namespace std::chrono;
    using namespace std::literals;

#if _WIN32
    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        const std::unordered_map<std::string_view, std::string_view> g_zoneMapping = []
        {
            struct Rec
            {
                std::string_view ws;
                std::string_view wd;
                std::string_view iana;
            };

            constexpr Rec recs[] =
            {
                {"Afghanistan Standard Time", "Afghanistan Daylight Time", "Asia/Kabul"},
                {"Alaskan Standard Time", "Alaskan Daylight Time", "America/Anchorage"},
                {"Aleutian Standard Time", "Aleutian Daylight Time", "America/Adak"},
                {"Altai Standard Time", "Altai Daylight Time", "Asia/Barnaul"},
                {"Arab Standard Time", "Arab Daylight Time", "Asia/Riyadh"},
                {"Arabian Standard Time", "Arabian Daylight Time", "Asia/Dubai"},
                {"Arabic Standard Time", "Arabic Daylight Time", "Asia/Baghdad"},
                {"Argentina Standard Time", "Argentina Daylight Time", "America/Buenos_Aires"},
                {"Armenian Standard Time", "Armenian Daylight Time", "Asia/Yerevan"},
                {"Astrakhan Standard Time", "Astrakhan Daylight Time", "Europe/Astrakhan"},
                {"Atlantic Standard Time", "Atlantic Daylight Time", "America/Halifax"},
                {"AUS Central Standard Time", "AUS Central Daylight Time", "Australia/Darwin"},
                {"Aus Central W. Standard Time", "Aus Central W. Daylight Time", "Australia/Eucla"},
                {"AUS Eastern Standard Time", "AUS Eastern Daylight Time", "Australia/Sydney"},
                {"Azerbaijan Standard Time", "Azerbaijan Daylight Time", "Asia/Baku"},
                {"Azores Standard Time", "Azores Daylight Time", "Atlantic/Azores"},
                {"Bahia Standard Time", "Bahia Daylight Time", "America/Bahia"},
                {"Bangladesh Standard Time", "Bangladesh Daylight Time", "Asia/Dhaka"},
                {"Belarus Standard Time", "Belarus Daylight Time", "Europe/Minsk"},
                {"Bougainville Standard Time", "Bougainville Daylight Time", "Pacific/Bougainville"},
                {"Cabo Verde Standard Time", "Cabo Verde Daylight Time", "Atlantic/Cape_Verde"},
                {"Canada Central Standard Time", "Canada Central Daylight Time", "America/Regina"},
                {"Cape Verde Standard Time", "Cape Verde Daylight Time", "Atlantic/Cape_Verde"},
                {"Caucasus Standard Time", "Caucasus Daylight Time", "Asia/Yerevan"},
                {"Cen. Australia Standard Time", "Cen. Australia Daylight Time", "Australia/Adelaide"},
                {"Central America Standard Time", "Central America Daylight Time", "America/Guatemala"},
                {"Central Asia Standard Time", "Central Asia Daylight Time", "Asia/Almaty"},
                {"Central Brazilian Standard Time", "Central Brazilian Daylight Time", "America/Cuiaba"},
                {"Central Europe Standard Time", "Central Europe Daylight Time", "Europe/Budapest"},
                {"Central European Standard Time", "Central European Daylight Time", "Europe/Warsaw"},
                {"Central Pacific Standard Time", "Central Pacific Daylight Time", "Pacific/Guadalcanal"},
                {"Central Standard Time", "Central Daylight Time", "America/Chicago"},
                {"Central Standard Time (Mexico)", "Central Daylight Time (Mexico)", "America/Mexico_City"},
                {"Chatham Islands Standard Time", "Chatham Islands Daylight Time", "Pacific/Chatham"},
                {"China Standard Time", "China Daylight Time", "Asia/Shanghai"},
                {"Coordinated Universal Time", "Coordinated Universal Time", "UTC"},
                {"Cuba Standard Time", "Cuba Daylight Time", "America/Havana"},
                {"Dateline Standard Time", "Dateline Daylight Time", "Etc/GMT+12"},
                {"E. Africa Standard Time", "E. Africa Daylight Time", "Africa/Nairobi"},
                {"E. Australia Standard Time", "E. Australia Daylight Time", "Australia/Brisbane"},
                {"E. Europe Standard Time", "E. Europe Daylight Time", "Europe/Chisinau"},
                {"E. South America Standard Time", "E. South America Daylight Time", "America/Sao_Paulo"},
                {"Easter Island Standard Time", "Easter Island Daylight Time", "Pacific/Easter"},
                {"Eastern Standard Time", "Eastern Daylight Time", "America/New_York"},
                {"Eastern Standard Time (Mexico)", "Eastern Daylight Time (Mexico)", "America/Cancun"},
                {"Egypt Standard Time", "Egypt Daylight Time", "Africa/Cairo"},
                {"Ekaterinburg Standard Time", "Ekaterinburg Daylight Time", "Asia/Yekaterinburg"},
                {"Fiji Standard Time", "Fiji Daylight Time", "Pacific/Fiji"},
                {"FLE Standard Time", "FLE Daylight Time", "Europe/Kiev"},
                {"Georgian Standard Time", "Georgian Daylight Time", "Asia/Tbilisi"},
                {"GMT Standard Time", "GMT Daylight Time", "Europe/London"},
                {"Greenland Standard Time", "Greenland Daylight Time", "America/Godthab"},
                {"Greenwich Standard Time", "Greenwich Daylight Time", "Atlantic/Reykjavik"},
                {"GTB Standard Time", "GTB Daylight Time", "Europe/Bucharest"},
                {"Haiti Standard Time", "Haiti Daylight Time", "America/Port-au-Prince"},
                {"Hawaiian Standard Time", "Hawaiian Daylight Time", "Pacific/Honolulu"},
                {"India Standard Time", "India Daylight Time", "Asia/Calcutta"},
                {"Iran Standard Time", "Iran Daylight Time", "Asia/Tehran"},
                {"Israel Standard Time", "Israel Daylight Time", "Asia/Jerusalem"},
                {"Jerusalem Standard Time", "Jerusalem Daylight Time", "Asia/Jerusalem"},
                {"Jordan Standard Time", "Jordan Daylight Time", "Asia/Amman"},
                {"Kaliningrad Standard Time", "Kaliningrad Daylight Time", "Europe/Kaliningrad"},
                {"Kamchatka Standard Time", "Kamchatka Daylight Time", "Asia/Kamchatka"},
                {"Korea Standard Time", "Korea Daylight Time", "Asia/Seoul"},
                {"Libya Standard Time", "Libya Daylight Time", "Africa/Tripoli"},
                {"Line Islands Standard Time", "Line Islands Daylight Time", "Pacific/Kiritimati"},
                {"Lord Howe Standard Time", "Lord Howe Daylight Time", "Australia/Lord_Howe"},
                {"Magadan Standard Time", "Magadan Daylight Time", "Asia/Magadan"},
                {"Magallanes Standard Time", "Magallanes Daylight Time", "America/Punta_Arenas"},
                {"Malay Peninsula Standard Time", "Malay Peninsula Daylight Time", "Asia/Kuala_Lumpur"},
                {"Marquesas Standard Time", "Marquesas Daylight Time", "Pacific/Marquesas"},
                {"Mauritius Standard Time", "Mauritius Daylight Time", "Indian/Mauritius"},
                {"Mexico Standard Time", "Mexico Daylight Time", "America/Mexico_City"},
                {"Mexico Standard Time 2", "Mexico Daylight Time 2", "America/Chihuahua"},
                {"Mid-Atlantic Standard Time", "Mid-Atlantic Daylight Time", "Atlantic/South_Georgia"},
                {"Middle East Standard Time", "Middle East Daylight Time", "Asia/Beirut"},
                {"Montevideo Standard Time", "Montevideo Daylight Time", "America/Montevideo"},
                {"Morocco Standard Time", "Morocco Daylight Time", "Africa/Casablanca"},
                {"Mountain Standard Time", "Mountain Daylight Time", "America/Denver"},
                {"Mountain Standard Time (Mexico)", "Mountain Daylight Time (Mexico)", "America/Chihuahua"},
                {"Myanmar Standard Time", "Myanmar Daylight Time", "Asia/Rangoon"},
                {"N. Central Asia Standard Time", "N. Central Asia Daylight Time", "Asia/Novosibirsk"},
                {"Namibia Standard Time", "Namibia Daylight Time", "Africa/Windhoek"},
                {"Nepal Standard Time", "Nepal Daylight Time", "Asia/Katmandu"},
                {"New Zealand Standard Time", "New Zealand Daylight Time", "Pacific/Auckland"},
                {"Newfoundland Standard Time", "Newfoundland Daylight Time", "America/St_Johns"},
                {"Norfolk Standard Time", "Norfolk Daylight Time", "Pacific/Norfolk"},
                {"North Asia East Standard Time", "North Asia East Daylight Time", "Asia/Irkutsk"},
                {"North Asia Standard Time", "North Asia Daylight Time", "Asia/Krasnoyarsk"},
                {"North Korea Standard Time", "North Korea Daylight Time", "Asia/Pyongyang"},
                {"Novosibirsk Standard Time", "Novosibirsk Daylight Time", "Asia/Novosibirsk"},
                {"Omsk Standard Time", "Omsk Daylight Time", "Asia/Omsk"},
                {"Pacific SA Standard Time", "Pacific SA Daylight Time", "America/Santiago"},
                {"Pacific Standard Time", "Pacific Daylight Time", "America/Los_Angeles"},
                {"Pacific Standard Time (Mexico)", "Pacific Daylight Time (Mexico)", "America/Tijuana"},
                {"Pakistan Standard Time", "Pakistan Daylight Time", "Asia/Karachi"},
                {"Paraguay Standard Time", "Paraguay Daylight Time", "America/Asuncion"},
                {"Qyzylorda Standard Time", "Qyzylorda Daylight Time", "Asia/Qyzylorda"},
                {"Romance Standard Time", "Romance Daylight Time", "Europe/Paris"},
                {"Russia Time Zone 3", "Russia Time Zone 3", "Europe/Samara"},
                {"Russia Time Zone 10", "Russia Time Zone 10", "Asia/Srednekolymsk"},
                {"Russia Time Zone 11", "Russia Time Zone 11", "Asia/Kamchatka"},
                {"Russia TZ 1 Standard Time", "Russia TZ 1 Daylight Time", "Europe/Kaliningrad"},
                {"Russia TZ 2 Standard Time", "Russia TZ 2 Daylight Time", "Europe/Moscow"},
                {"Russia TZ 3 Standard Time", "Russia TZ 3 Daylight Time", "Europe/Samara"},
                {"Russia TZ 4 Standard Time", "Russia TZ 4 Daylight Time", "Asia/Yekaterinburg"},
                {"Russia TZ 5 Standard Time", "Russia TZ 5 Daylight Time", "Asia/Novosibirsk"},
                {"Russia TZ 6 Standard Time", "Russia TZ 6 Daylight Time", "Asia/Krasnoyarsk"},
                {"Russia TZ 7 Standard Time", "Russia TZ 7 Daylight Time", "Asia/Irkutsk"},
                {"Russia TZ 8 Standard Time", "Russia TZ 8 Daylight Time", "Asia/Yakutsk"},
                {"Russia TZ 9 Standard Time", "Russia TZ 9 Daylight Time", "Asia/Vladivostok"},
                {"Russia TZ 10 Standard Time", "Russia TZ 10 Daylight Time", "Asia/Magadan"},
                {"Russia TZ 11 Standard Time", "Russia TZ 11 Daylight Time", "Asia/Anadyr"},
                {"Russian Standard Time", "Russian Daylight Time", "Europe/Moscow"},
                {"SA Eastern Standard Time", "SA Eastern Daylight Time", "America/Cayenne"},
                {"SA Pacific Standard Time", "SA Pacific Daylight Time", "America/Bogota"},
                {"SA Western Standard Time", "SA Western Daylight Time", "America/La_Paz"},
                {"Saint Pierre Standard Time", "Saint Pierre Daylight Time", "America/Miquelon"},
                {"Sakhalin Standard Time", "Sakhalin Daylight Time", "Asia/Sakhalin"},
                {"Samoa Standard Time", "Samoa Daylight Time", "Pacific/Apia"},
                {"Sao Tome Standard Time", "Sao Tome Daylight Time", "Africa/Sao_Tome"},
                {"Saratov Standard Time", "Saratov Daylight Time", "Europe/Saratov"},
                {"SE Asia Standard Time", "SE Asia Daylight Time", "Asia/Bangkok"},
                {"Singapore Standard Time", "Singapore Daylight Time", "Asia/Singapore"},
                {"South Africa Standard Time", "South Africa Daylight Time", "Africa/Johannesburg"},
                {"South Sudan Standard Time", "South Sudan Daylight Time", "Africa/Juba"},
                {"Sri Lanka Standard Time", "Sri Lanka Daylight Time", "Asia/Colombo"},
                {"Sudan Standard Time", "Sudan Daylight Time", "Africa/Khartoum"},
                {"Syria Standard Time", "Syria Daylight Time", "Asia/Damascus"},
                {"Taipei Standard Time", "Taipei Daylight Time", "Asia/Taipei"},
                {"Tasmania Standard Time", "Tasmania Daylight Time", "Australia/Hobart"},
                {"Tocantins Standard Time", "Tocantins Daylight Time", "America/Araguaina"},
                {"Tokyo Standard Time", "Tokyo Daylight Time", "Asia/Tokyo"},
                {"Tomsk Standard Time", "Tomsk Daylight Time", "Asia/Tomsk"},
                {"Tonga Standard Time", "Tonga Daylight Time", "Pacific/Tongatapu"},
                {"Transbaikal Standard Time", "Transbaikal Daylight Time", "Asia/Chita"},
                {"Turkey Standard Time", "Turkey Daylight Time", "Europe/Istanbul"},
                {"Turks And Caicos Standard Time", "Turks And Caicos Daylight Time", "America/Grand_Turk"},
                {"Ulaanbaatar Standard Time", "Ulaanbaatar Daylight Time", "Asia/Ulaanbaatar"},
                {"US Eastern Standard Time", "US Eastern Daylight Time", "America/Indianapolis"},
                {"US Mountain Standard Time", "US Mountain Daylight Time", "America/Phoenix"},
                {"UTC", "UTC", "UTC"},
                {"UTC+12", "UTC+12", "Etc/GMT-12"},
                {"UTC+13", "UTC+13", "Etc/GMT-13"},
                {"UTC-02", "UTC-02", "Etc/GMT+2"},
                {"UTC-08", "UTC-08", "Etc/GMT+8"},
                {"UTC-09", "UTC-09", "Etc/GMT+9"},
                {"UTC-11", "UTC-11", "Etc/GMT+11"},
                {"Venezuela Standard Time", "Venezuela Daylight Time", "America/Caracas"},
                {"Vladivostok Standard Time", "Vladivostok Daylight Time", "Asia/Vladivostok"},
                {"Volgograd Standard Time", "Volgograd Daylight Time", "Europe/Volgograd"},
                {"W. Australia Standard Time", "W. Australia Daylight Time", "Australia/Perth"},
                {"W. Central Africa Standard Time", "W. Central Africa Daylight Time", "Africa/Lagos"},
                {"W. Europe Standard Time", "W. Europe Daylight Time", "Europe/Berlin"},
                {"W. Mongolia Standard Time", "W. Mongolia Daylight Time", "Asia/Hovd"},
                {"West Asia Standard Time", "West Asia Daylight Time", "Asia/Tashkent"},
                {"West Bank Gaza Standard Time", "West Bank Gaza Daylight Time", "Asia/Gaza"},
                {"West Bank Standard Time", "West Bank Daylight Time", "Asia/Hebron"},
                {"West Pacific Standard Time", "West Pacific Daylight Time", "Pacific/Port_Moresby"},
                {"Yakutsk Standard Time", "Yakutsk Daylight Time", "Asia/Yakutsk"},
                {"Yukon Standard Time", "Yukon Daylight Time", "America/Whitehorse"},
            };

            std::unordered_map<std::string_view, std::string_view> result;
            for(const Rec& rec : recs)
            {
                result.emplace(rec.ws, rec.iana);
                result.emplace(rec.wd, rec.iana);
            }
            return result;
        }();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const time_zone* currentZone()
    {
        static const time_zone* result = []
        {
            std::string zoneName;

            TIME_ZONE_INFORMATION tzi{};
            switch(GetTimeZoneInformation(&tzi))
            {
            default:
            case TIME_ZONE_ID_INVALID:
                return current_zone();
            case TIME_ZONE_ID_UNKNOWN:
            case TIME_ZONE_ID_STANDARD:
                zoneName = str::wideConvert(tzi.StandardName);
                break;
            case TIME_ZONE_ID_DAYLIGHT:
                zoneName = str::wideConvert(tzi.DaylightName);
                break;
            }

            auto iter = g_zoneMapping.find(zoneName);
            if(g_zoneMapping.end() == iter)
                return current_zone();

            return get_tzdb().locate_zone(iter->second);
        }();

        return result;
    }
#else
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const time_zone* currentZone()
    {
        return current_zone();
    }
#endif

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        const time_zone* localZone{currentZone()};
        const time_zone* utcZone{locate_zone("UTC"sv)};

        using TPd = time_point<system_clock, duration<double>>;

        template <class Duration>
        using TP = time_point<system_clock, Duration>;

        template <class Duration>
        using TPZ = zoned_time<Duration>;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    double str2Moment(std::string_view str, const time_zone* targetZone)
    {
        TPd tpd;
        if( (std::ispanstream{str} >> parse("%FT%T%z", tpd)) ||
            (std::ispanstream{str} >> parse("%F %T%z", tpd)))
        {
            return tpd.time_since_epoch().count();
        }

        if( (std::ispanstream{str} >> parse("%FT%T"  , tpd)) ||
            (std::ispanstream{str} >> parse("%F %T"  , tpd)) ||
            (std::ispanstream{str} >> parse("%F"     , tpd)))
        {
            auto localOffset = localZone->get_info(tpd).offset;
            tpd -= localOffset;

            if(!targetZone)
                targetZone = utcZone;
            auto targetOffset = targetZone->get_info(tpd).offset;
            tpd += (targetOffset - localOffset);

            return tpd.time_since_epoch().count();
        }

        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void moment2Datetime6Tz(double epochSeconds, std::string& dst, const time_zone* targetZone)
    {
        if(!targetZone)
            targetZone = utcZone;
        TPZ tpz{targetZone, time_point_cast<microseconds>(TPd{duration<double>{epochSeconds}})};
        dst = std::format("{:%F %T%Ez}", tpz);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void moment2Datetime3Tz(double epochSeconds, std::string& dst, const time_zone* targetZone)
    {
        if(!targetZone)
            targetZone = utcZone;
        TPZ tpz{targetZone, time_point_cast<milliseconds>(TPd{duration<double>{epochSeconds}})};
        dst = std::format("{:%F %T%Ez}", tpz);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void moment2DatetimeTz(double epochSeconds, std::string& dst, const time_zone* targetZone)
    {
        if(!targetZone)
            targetZone = utcZone;
        TPZ tpz{targetZone, time_point_cast<seconds>(TPd{duration<double>{epochSeconds}})};
        dst = std::format("{:%F %T%Ez}", tpz);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void moment2Datetime6(double epochSeconds, std::string& dst, const time_zone* targetZone)
    {
        if(!targetZone)
            targetZone = utcZone;
        TPZ tpz{targetZone, time_point_cast<microseconds>(TPd{duration<double>{epochSeconds}})};
        dst = std::format("{:%F %T}", tpz);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void moment2Datetime3(double epochSeconds, std::string& dst, const time_zone* targetZone)
    {
        if(!targetZone)
            targetZone = utcZone;
        TPZ tpz{targetZone, time_point_cast<milliseconds>(TPd{duration<double>{epochSeconds}})};
        dst = std::format("{:%F %T}", tpz);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void moment2Datetime(double epochSeconds, std::string& dst, const time_zone* targetZone)
    {
        if(!targetZone)
            targetZone = utcZone;
        TPZ tpz{targetZone, time_point_cast<seconds>(TPd{duration<double>{epochSeconds}})};
        dst = std::format("{:%F %T}", tpz);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void moment2Date(double epochSeconds, std::string& dst, const time_zone* targetZone)
    {
        if(!targetZone)
            targetZone = utcZone;
        TPZ tpz{targetZone, time_point_cast<seconds>(TPd{duration<double>{epochSeconds}})};
        dst = std::format("{:%F}", tpz);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string moment2Datetime6Tz(double epochSeconds, const time_zone* targetZone)
    {
        std::string res;
        moment2Datetime6Tz(epochSeconds, res, targetZone);
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string moment2Datetime3Tz(double epochSeconds, const time_zone* targetZone)
    {
        std::string res;
        moment2Datetime3Tz(epochSeconds, res, targetZone);
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string moment2DatetimeTz(double epochSeconds, const time_zone* targetZone)
    {
        std::string res;
        moment2DatetimeTz(epochSeconds, res, targetZone);
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string moment2Datetime6(double epochSeconds, const time_zone* targetZone)
    {
        std::string res;
        moment2Datetime6(epochSeconds, res, targetZone);
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string moment2Datetime3(double epochSeconds, const time_zone* targetZone)
    {
        std::string res;
        moment2Datetime3(epochSeconds, res, targetZone);
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string moment2Datetime(double epochSeconds, const time_zone* targetZone)
    {
        std::string res;
        moment2Datetime(epochSeconds, res, targetZone);
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string moment2Date(double epochSeconds, const time_zone* targetZone)
    {
        std::string res;
        moment2Date(epochSeconds, res, targetZone);
        return res;
    }
}
