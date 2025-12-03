include(dciIntegrationUnit)
dciIntegrationUnit(poll
        INCLUDE_DIRS include
        DEPENDS cmt)

############################################################
file(GLOB_RECURSE INC include/*)
file(GLOB_RECURSE SRC src/*)

if(WIN32)
    list(FILTER SRC EXCLUDE REGEX ".*polling.epoll.(hpp|cpp)$")
else()
    list(FILTER SRC EXCLUDE REGEX ".*polling.asyncSelect.(hpp|cpp)$")
endif()

add_library(${UNAME} SHARED ${INC} ${SRC})
dciIntegrationSetupTarget(${UNAME})
target_include_directories(${UNAME} PRIVATE src)

target_link_libraries(${UNAME} PRIVATE
    mm
    cmt
    sbs
    logger
    exception
    utils
)

if(WIN32)
    target_link_libraries(${UNAME} PRIVATE Ws2_32.lib)
else()
    include(CheckSymbolExists)
    check_symbol_exists(epoll_pwait2 "sys/epoll.h" HAVE_EPOLL_PWAIT2)
    if(HAVE_EPOLL_PWAIT2)
        target_compile_definitions(${UNAME} PRIVATE -DHAVE_EPOLL_PWAIT2=1)
    endif()
endif()

############################################################
include(dciHimpl)
dciHimplMakeLayouts(${UNAME}
    INCLUDE
        src

    OUTPUT
        dci/poll/implMetaInfo.hpp

    HEADERS
        impl/descriptor.hpp
        impl/timer.hpp
        impl/awaker.hpp

    CLASSES
        dci::poll::impl::Descriptor
        dci::poll::impl::Timer
        dci::poll::impl::Awaker

    LINK cmt exception
)

############################################################
file(GLOB_RECURSE TST test/*)
include(dciTest)
dciTest(${UNAME} noenv
    SRC ${TST}
    LINK ${UNAME} utils cmt sbs exception mm logger
)

if(WIN32 AND TARGET ${UNAME}-test-noenv)
    target_link_libraries(${UNAME}-test-noenv PRIVATE Ws2_32.lib)
endif()
