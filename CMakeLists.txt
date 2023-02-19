cmake_minimum_required(VERSION 3.20)
project(dci)

###################################################################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-backtrace-limit=10")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmax-errors=10")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ferror-limit=10")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wno-error")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-braces")


#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")

#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat-pedantic")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-exit-time-destructors")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow-field")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch-enum")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-documentation")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-float-equal")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-weak-vtables")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-weak-template-vtables")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-extra-semi-stmt")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unreachable-code-return")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unreachable-code-break")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-covered-switch-default")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-template")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-undefined-func-template")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-padded")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-alloca")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-ctad-maybe-unsupported")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-header-hygiene")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-redundant-parens")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-variable-declarations")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-disabled-macro-expansion")
endif()


###################################################################################
set(CMAKE_CXX_FLAGS_DEBUG)
set(CMAKE_C_FLAGS_DEBUG)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb3")

###################################################################################
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=x86-64 -UNDEBUG -Ofast")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=x86-64 -UNDEBUG -Ofast")

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE On)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO On)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ffunction-sections -fdata-sections")

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,--gc-sections")
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} -Wl,--gc-sections")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -Wl,--gc-sections")
#set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS_RELEASE} ")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address --param asan-stack=0 -fno-omit-frame-pointer")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address --param asan-stack=0 -fno-omit-frame-pointer")

##########################################################################################
file(GLOB units1 ${CMAKE_CURRENT_LIST_DIR}/source/core/*/CMakeLists.txt)
file(GLOB units2 ${CMAKE_CURRENT_LIST_DIR}/source/module/*/CMakeLists.txt)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/source/core/integration/cmakeModules)
include(dciIntegration)
dciIntegration(
    UNIT_SCRIPTS            ${units1} ${units2}
#    SRC_DIR                 ${CMAKE_CURRENT_SOURCE_DIR}

#    OUT_DIR                 $ENV{DCI_OUT_DIR}
#    BE_DIR                  $ENV{DCI_BE_DIR}
#    SRC_BRANCH              $ENV{DCI_SRC_BRANCH}
#    SRC_REVISION            $ENV{DCI_SRC_REVISION}
#    SRC_MOMENT              $ENV{DCI_SRC_MOMENT}
#    PLATFORM_OS             $ENV{DCI_PLATFORM_OS}
#    PLATFORM_ARCH           $ENV{DCI_PLATFORM_ARCH}
#    COMPILER                $ENV{DCI_COMPILER}
#    COMPILER_VERSION        $ENV{DCI_COMPILER_VERSION}
#    COMPILER_OPTIMIZATION   $ENV{DCI_COMPILER_OPTIMIZATION}
#    REPRODUCIBLE_BUILD      $ENV{DCI_REPRODUCIBLE_BUILD}
#    PROVIDER                $ENV{DCI_PROVIDER}
#    AUP_SIGNERKEY           $ENV{DCI_AUP_SIGNERKEY}

#    BUILD_TESTS             On
#    STRIP_DEBUG             Off
#    SEPARATE_DEBUG          Off
)
