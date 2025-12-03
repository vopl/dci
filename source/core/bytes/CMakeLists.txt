include(dciIntegrationUnit)
dciIntegrationUnit(bytes
        INCLUDE_DIRS include
        DEPENDS test primitives mm)

##############################################################
file(GLOB_RECURSE INC include/*)
file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE TST test/*)

add_library(${UNAME} SHARED ${INC} ${SRC})
dciIntegrationSetupTarget(${UNAME})
target_include_directories(${UNAME} PRIVATE src)

target_link_libraries(${UNAME} PRIVATE
    utils
    mm
)

############################################################
include(dciHimpl)
dciHimplMakeLayouts(${UNAME}
    INCLUDE
        src

    OUTPUT
        dci/bytes/implMetaInfo.hpp

    HEADERS
        impl/bytes.hpp
        bytes/impl/cursor.hpp
        bytes/impl/alter.hpp

    CLASSES
        dci::impl::Bytes
        dci::bytes::impl::Cursor
        dci::bytes::impl::Alter
)

##############################################################
include(dciTest)
dciTest(${UNAME} noenv
    SRC
        ${TST}
    LINK
        ${UNAME}
        utils
        mm
)
