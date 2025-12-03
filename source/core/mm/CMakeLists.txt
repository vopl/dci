include(dciIntegrationUnit)
dciIntegrationUnit(mm
        INCLUDE_DIRS include
        DEPENDS himpl utils)

############################################################
include(cmakeModules/FindValgrind.cmake)
if(VALGRIND_FOUND)
    set(HAVE_VALGRIND TRUE)
    message(STATUS "found valgrind: ${VALGRIND_PROGRAM}, ${VALGRIND_INCLUDE_DIR}")
endif()

############################################################
file(GLOB_RECURSE INC include/*)
file(GLOB_RECURSE SRC src/*)

if(WIN32)
    list(FILTER SRC EXCLUDE REGEX .*vm-posix.cpp$)
else()
    list(FILTER SRC EXCLUDE REGEX .*vm-win.cpp$)
endif()

add_library(${UNAME} SHARED ${INC} ${SRC})
dciIntegrationSetupTarget(${UNAME})
target_include_directories(${UNAME} PRIVATE src)
if(VALGRIND_FOUND)
    target_include_directories(${UNAME} SYSTEM PRIVATE ${VALGRIND_INCLUDE_DIR})
endif()
target_link_libraries(${UNAME} PRIVATE utils)

############################################################
#TODO: determine real values

set(DCIMMCONFIG_pageSize                    4096    )#detect
set(DCIMMCONFIG_cachelineSize               64      )#detect

set(DCIMMCONFIG_stackPages                  32      )# 4096*32 = 128Kbytes
set(DCIMMCONFIG_stackGrowsDown              true    )#detect
set(DCIMMCONFIG_stackHasGuard               true    )

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DCIMMCONFIG_stackKeepProtectedBytes _stackPages*_pageSize)
else()
    set(DCIMMCONFIG_stackKeepProtectedBytes 2048    )
endif()

set(DCIMMCONFIG_stacksAmount                1024*1024*512)

configure_file(src/config.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/src/config.hpp @ONLY)
target_include_directories(${UNAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

############################################################
include(dciHimpl)
dciHimplMakeLayouts(${UNAME}
    INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}/src

    OUTPUT
        dci/mm/implMetaInfo.hpp

    HEADERS
        impl/stack.hpp

    CLASSES
        dci::mm::impl::Stack
)
