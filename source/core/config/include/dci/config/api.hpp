// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/integration/apiDecls.hpp>

#ifdef DCI_CONFIG_EXPORTS
#   define API_DCI_CONFIG DCI_INTEGRATION_APIDECL_EXPORT
#else
#   define API_DCI_CONFIG DCI_INTEGRATION_APIDECL_IMPORT
#endif
