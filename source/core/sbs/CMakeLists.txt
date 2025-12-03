include(dciIntegrationUnit)
dciIntegrationUnit(sbs
        INCLUDE_DIRS include
        DEPENDS test exception mm)

############################################################
file(GLOB_RECURSE INC include/*)
file(GLOB_RECURSE SRC src/*)

add_library(${UNAME} SHARED ${INC} ${SRC})
dciIntegrationSetupTarget(${UNAME})
target_link_libraries(${UNAME} PRIVATE utils)

##############################################################
include(dciHimpl)
dciHimplMakeLayouts(${UNAME}
    INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/src

    OUTPUT
        dci/sbs/implMetaInfo.hpp

    HEADERS
        impl/box.hpp
        impl/owner.hpp
        impl/subscription.hpp

    CLASSES
        dci::sbs::impl::Box
        dci::sbs::impl::Owner
        dci::sbs::impl::Subscription
)

############################################################
file(GLOB_RECURSE TST test/*)
include(dciTest)
dciTest(${UNAME} noenv
    SRC ${TST}
    LINK ${UNAME} mm utils exception
)
