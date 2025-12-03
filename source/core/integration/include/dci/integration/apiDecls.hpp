// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#if __GNUC__
#   if _WIN32
#       define DCI_INTEGRATION_APIDECL_LOCAL
#       define DCI_INTEGRATION_APIDECL_EXPORT __declspec(dllexport)
#       define DCI_INTEGRATION_APIDECL_IMPORT __declspec(dllimport)
#   else
#       define DCI_INTEGRATION_APIDECL_LOCAL  __attribute__((visibility("hidden")))
#       define DCI_INTEGRATION_APIDECL_EXPORT __attribute__((visibility("default")))
#       define DCI_INTEGRATION_APIDECL_IMPORT
#   endif
#elif _MSC_VER
#   define DCI_INTEGRATION_APIDECL_LOCAL
#   define DCI_INTEGRATION_APIDECL_EXPORT __declspec(dllexport)
#   define DCI_INTEGRATION_APIDECL_IMPORT __declspec(dllimport)
#else
#   pragma message "unsupported compiler for binary exports"
#   define DCI_INTEGRATION_APIDECL_LOCAL
#   define DCI_INTEGRATION_APIDECL_EXPORT
#   define DCI_INTEGRATION_APIDECL_IMPORT
#endif
