include(dciIntegrationUnit)
dciIntegrationUnit(cmt
        INCLUDE_DIRS include
        DEPENDS test sbs logger utils)

############################################################
include(CheckIncludeFiles)

############################################################
check_include_files(ucontext.h HAVE_UCONTEXT)

if(HAVE_UCONTEXT)
    message(STATUS "Found ucontext.h")
endif()

############################################################
if(WIN32)
    include(CheckPrototypeDefinition)
    check_prototype_definition(
        CreateFiberEx
        "LPVOID WINAPI CreateFiberEx(SIZE_T dwStackCommitSize, SIZE_T dwStackReserveSize, DWORD dwFlags, LPFIBER_START_ROUTINE lpStartAddress, LPVOID lpParameter);"
        "NULL"
        windows.h
        HAVE_WINFIBER)
endif()

############################################################
find_package(Boost REQUIRED COMPONENTS context)
set(HAVE_BOOSTCONTEXT ON)


############################################################
if(NOT DCI_CMT_CONTEXTENGINE_WINFIBER AND NOT DCI_CMT_CONTEXTENGINE_UCONTEXT AND NOT DCI_CMT_CONTEXTENGINE_BOOSTCONTEXT)
    if(HAVE_BOOSTCONTEXT)
        set(DCI_CMT_CONTEXTENGINE_BOOSTCONTEXT ON)
    elseif(HAVE_WINFIBER)
        set(DCI_CMT_CONTEXTENGINE_WINFIBER ON)
    elseif(HAVE_UCONTEXT)
        set(DCI_CMT_CONTEXTENGINE_UCONTEXT ON)
    else()
        message(FATAL_ERROR "no support for context engine available")
    endif()
endif()



set(CONTEXT_ENGINE "")

if(DCI_CMT_CONTEXTENGINE_BOOSTCONTEXT AND HAVE_BOOSTCONTEXT)
    set(CONTEXT_ENGINE "BOOSTCONTEXT")
elseif(DCI_CMT_CONTEXTENGINE_UCONTEXT AND HAVE_UCONTEXT)
    set(CONTEXT_ENGINE "UCONTEXT")
elseif(DCI_CMT_CONTEXTENGINE_WINFIBER AND HAVE_WINFIBER)
    set(CONTEXT_ENGINE "WINFIBER")
else()
    message(FATAL_ERROR "context engine has no support")
endif()

message(STATUS "use ${CONTEXT_ENGINE} as context engine")
if(CONTEXT_ENGINE STREQUAL "WINFIBER")
    set(DCI_CMT_CONTEXTENGINE_WINFIBER ON)
    set(DCI_CMT_CONTEXTENGINE_UCONTEXT OFF)
    set(DCI_CMT_CONTEXTENGINE_BOOSTCONTEXT OFF)
elseif(CONTEXT_ENGINE STREQUAL "UCONTEXT")
    set(DCI_CMT_CONTEXTENGINE_WINFIBER OFF)
    set(DCI_CMT_CONTEXTENGINE_UCONTEXT ON)
    set(DCI_CMT_CONTEXTENGINE_BOOSTCONTEXT OFF)
elseif(CONTEXT_ENGINE STREQUAL "BOOSTCONTEXT")
    set(DCI_CMT_CONTEXTENGINE_WINFIBER OFF)
    set(DCI_CMT_CONTEXTENGINE_UCONTEXT OFF)
    set(DCI_CMT_CONTEXTENGINE_BOOSTCONTEXT ON)
else()
    message(FATAL_ERROR "unknown context engine2")
endif()


set(DCI_CMT_CONTEXTENGINE_WINFIBER ${DCI_CMT_CONTEXTENGINE_WINFIBER} CACHE BOOL "use windows fibers as context engine" FORCE)
set(DCI_CMT_CONTEXTENGINE_UCONTEXT ${DCI_CMT_CONTEXTENGINE_UCONTEXT} CACHE BOOL "use posix ucontext as context engine" FORCE)
set(DCI_CMT_CONTEXTENGINE_BOOSTCONTEXT ${DCI_CMT_CONTEXTENGINE_BOOSTCONTEXT} CACHE BOOL "use boost::context as context engine" FORCE)

############################################################
configure_file(src/config.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/src/config.hpp)



















############################################################
file(GLOB_RECURSE INC include/*)
file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE TST test/*)

list(FILTER SRC EXCLUDE REGEX impl/ctx/engine_)

if(DCI_CMT_CONTEXTENGINE_WINFIBER)
    list(APPEND SRC src/impl/ctx/engine_winfiber.hpp)
endif()
if(DCI_CMT_CONTEXTENGINE_UCONTEXT)
    list(APPEND SRC src/impl/ctx/engine_ucontext.hpp)
endif()
if(DCI_CMT_CONTEXTENGINE_BOOSTCONTEXT)
    list(APPEND SRC src/impl/ctx/engine_boostcontext.hpp)
endif()



















############################################################
add_library(${UNAME} SHARED ${INC} ${SRC})
dciIntegrationSetupTarget(${UNAME})
target_include_directories(${UNAME} PRIVATE src)
target_include_directories(${UNAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

if(DCI_CMT_CONTEXTENGINE_BOOSTCONTEXT)
    target_link_libraries(${UNAME} PRIVATE Boost::context)
endif()

target_link_libraries(${UNAME} PRIVATE
    exception
    mm
    sbs
    utils
    logger
)

include(dciHimpl)
dciHimplMakeLayouts(${UNAME}
    INCLUDE
        src
        ${CMAKE_CURRENT_BINARY_DIR}/src

    OUTPUT
        dci/cmt/implMetaInfo.hpp

    HEADERS
        impl/details/waiter.hpp

        impl/task/face.hpp
        impl/task/body.hpp
        impl/task/owner.hpp

        impl/waitable.hpp
        impl/raisable.hpp
        impl/lockable.hpp
        impl/event.hpp
        impl/notifier.hpp
        impl/pulser.hpp
        impl/mutex.hpp
        impl/semaphore.hpp


    CLASSES
        dci::cmt::impl::details::Waiter

        dci::cmt::impl::task::Face
        dci::cmt::impl::task::Body
        dci::cmt::impl::task::Owner

        dci::cmt::impl::Raisable
        dci::cmt::impl::Waitable
        dci::cmt::impl::Lockable
        dci::cmt::impl::Event
        dci::cmt::impl::Notifier
        dci::cmt::impl::Pulser
        dci::cmt::impl::Mutex
        dci::cmt::impl::Semaphore
)

############################################################
include(dciTest)
dciTest(${UNAME} noenv
    SRC ${TST}
    LINK ${UNAME} utils exception sbs logger mm
)
