include(dciIntegrationUnit)
dciIntegrationUnit(host
        INCLUDE_DIRS include
        CMM_DIRS cmakeModules
        IDL_DIRS idl
        DEPENDS poll aup config stiac)

############################################################
file(GLOB_RECURSE INC include/*)
file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE IDL idl/*)
file(GLOB_RECURSE CMM cmakeModules/*)

list(FILTER SRC EXCLUDE REGEX ".*main.cpp$")

############################################################
add_library(${UNAME}-lib SHARED ${INC} ${SRC} ${IDL} ${CMM})
set_target_properties(${UNAME}-lib PROPERTIES OUTPUT_NAME ${UNAME})
dciIntegrationSetupTarget(${UNAME}-lib)
#target_include_directories(${UNAME}-lib PRIVATE src)

target_link_libraries(${UNAME}-lib PRIVATE
#    himpl
    mm
    cmt
    idl
    bytes
    poll
    sbs
    test
    utils
    config
    stiac
    exception
    logger
)

if(UNIX)
    target_link_libraries(${UNAME}-lib PRIVATE dl)
endif()

find_package(Boost REQUIRED COMPONENTS program_options filesystem CONFIG)
target_link_libraries(${UNAME}-lib PRIVATE Boost::program_options Boost::filesystem)

############################################################
include(dciHimpl)
dciHimplMakeLayouts(${UNAME}-lib
    INCLUDE
        src

    OUTPUT
        dci/host/implMetaInfo.hpp

    HEADERS
        impl/manager.hpp

    CLASSES
        dci::host::impl::Manager
)

##############################################################
include(dciIdl)
dciIdl(${UNAME}-lib cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES host.idl
    OPTIONS --cpp-stiac-support --cpp-stiac-support-filter "^::dci::idl::gen::host::"
    NAME idl-host
)

################################################################
add_executable(${UNAME} src/main.cpp)
dciIntegrationSetupTarget(${UNAME})
target_link_libraries(${UNAME} PRIVATE
    ${UNAME}-lib
    mm
    cmt
    sbs
    exception
    aup
    poll
    integration
    logger
    Boost::program_options
)

if(WIN32)
    target_link_libraries(${UNAME} PRIVATE Dbghelp.lib)
endif()

############################################################
add_test(NAME noenv COMMAND ${UNAME} --test noenv)
add_test(NAME mnone COMMAND ${UNAME} --test mnone)
add_test(NAME mstart COMMAND ${UNAME} --test mstart)
