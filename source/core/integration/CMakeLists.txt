include(dciIntegrationUnit)
dciIntegrationUnit(integration
        INCLUDE_DIRS include
        CMM_DIRS cmakeModules)

file(GLOB_RECURSE INC include/*)
file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE CMM cmakeModules/*)

add_library(${UNAME} SHARED
    ${INC}
    ${SRC}
    ${CMM}
)

dciIntegrationSetupTarget(${UNAME})

#####################
target_compile_definitions(${UNAME} PRIVATE -DDCI_SRC_BRANCH="${DCI_SRC_BRANCH}")
target_compile_definitions(${UNAME} PRIVATE -DDCI_SRC_REVISION="${DCI_SRC_REVISION}")
target_compile_definitions(${UNAME} PRIVATE -DDCI_SRC_MOMENT=${DCI_SRC_MOMENT})
target_compile_definitions(${UNAME} PRIVATE -DDCI_PLATFORM_OS="${DCI_PLATFORM_OS}")
target_compile_definitions(${UNAME} PRIVATE -DDCI_PLATFORM_ARCH="${DCI_PLATFORM_ARCH}")
target_compile_definitions(${UNAME} PRIVATE -DDCI_COMPILER="${DCI_COMPILER}")
target_compile_definitions(${UNAME} PRIVATE -DDCI_COMPILER_VERSION="${DCI_COMPILER_VERSION}")
target_compile_definitions(${UNAME} PRIVATE -DDCI_COMPILER_OPTIMIZATION="${DCI_COMPILER_OPTIMIZATION}")
target_compile_definitions(${UNAME} PRIVATE -DDCI_VENDOR="${DCI_VENDOR}")

#####################
include(dciIntegrationMeta)

dciIntegrationMeta(UNIT ${UNAME} SRC_FILE ${CMAKE_SOURCE_DIR}/CMakeLists.txt CMakeLists.txt)
dciIntegrationMeta(UNIT ${UNAME} CMM_FILE ${CMAKE_SOURCE_DIR}/CMakeLists.txt CMakeLists.txt)
dciIntegrationMeta(UNIT ${UNAME} SRC_DIR ${CMAKE_SOURCE_DIR}/source/make source/make)

dciIntegrationMeta(UNIT ${UNAME} EXTRA_ALLOWED "etc/**")
dciIntegrationMeta(UNIT ${UNAME} EXTRA_ALLOWED "log/**")
dciIntegrationMeta(UNIT ${UNAME} EXTRA_ALLOWED "var/**")
dciIntegrationMeta(UNIT ${UNAME} EXTRA_ALLOWED "tmp/**")

dciIntegrationMeta(UNIT ${UNAME} EXTRA_ALLOWED ".qtcreator/**")
dciIntegrationMeta(UNIT ${UNAME} EXTRA_ALLOWED ".git/**")
dciIntegrationMeta(UNIT ${UNAME} EXTRA_ALLOWED "txt/**")

dciIntegrationMeta(UNIT ${UNAME} EXTRA_ALLOWED "include/**")
dciIntegrationMeta(UNIT ${UNAME} EXTRA_ALLOWED "idl/**")
dciIntegrationMeta(UNIT ${UNAME} EXTRA_ALLOWED "cmakeModules/**")
dciIntegrationMeta(UNIT ${UNAME} EXTRA_ALLOWED "source/**")

dciIntegrationMeta(UNIT ${UNAME} EXTRA_ALLOWED "*")
