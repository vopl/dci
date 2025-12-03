include(dciIntegrationUnit)
dciIntegrationUnit(stiac
        INCLUDE_DIRS include
        DEPENDS idl exception logger)

##############################################################
file(GLOB_RECURSE INC include/*)
file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE TST test/*)

add_library(${UNAME} SHARED ${INC} ${SRC})
dciIntegrationSetupTarget(${UNAME})
target_include_directories(${UNAME} PRIVATE src)

target_link_libraries(${UNAME} PRIVATE
    logger
    utils
    bytes
    exception
)

############################################################
include(dciHimpl)
dciHimplMakeLayouts(${UNAME}
    INCLUDE
        src

    OUTPUT
        dci/stiac/implMetaInfo.hpp

    HEADERS
        stiac/link/impl/sink.hpp
        stiac/link/impl/source.hpp
        stiac/link/source/impl/fail.hpp

    CLASSES
        dci::stiac::link::impl::Sink
        dci::stiac::link::impl::Source
        dci::stiac::link::source::impl::Fail
)

##############################################################
include(dciTest)
dciTest(${UNAME} noenv
    SRC
        ${TST}
    LINK
        ${UNAME}
        utils
        exception
        bytes
)
