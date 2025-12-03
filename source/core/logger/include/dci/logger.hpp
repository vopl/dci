// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "logger/stream.hpp"
#include "logger/timeProvider.hpp"

#if !defined(dciLoggerIdentity)
#   if defined(dciModuleName)
#       define dciLoggerIdentity dciModuleName
#   elif defined(dciUnitName)
#       define dciLoggerIdentity dciUnitName
#   else
#       define dciLoggerIdentity ""
#   endif
#endif

#   define LOGF(...) dci::logger::Stream{"FTL", dciLoggerIdentity} << __VA_ARGS__
#   define LOGE(...) dci::logger::Stream{"ERR", dciLoggerIdentity} << __VA_ARGS__
#   define LOGW(...) dci::logger::Stream{"WRN", dciLoggerIdentity} << __VA_ARGS__
#   define LOGI(...) dci::logger::Stream{"INF", dciLoggerIdentity} << __VA_ARGS__
#   define LOGD(...) dci::logger::Stream{"DBG", dciLoggerIdentity} << __VA_ARGS__
#   define LOGT(...) dci::logger::Stream{"TRC", dciLoggerIdentity} << __VA_ARGS__
