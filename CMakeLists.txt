cmake_minimum_required(VERSION 3.20)
project(dci)

###################################################################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-backtrace-limit=0")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmax-errors=10")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ferror-limit=10")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wno-error")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-braces")
endif()


###################################################################################
set(CMAKE_CXX_FLAGS_DEBUG)
set(CMAKE_C_FLAGS_DEBUG)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=x86-64 -ggdb3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=x86-64 -ggdb3")

###################################################################################
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Ofast")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wa,-mbig-obj")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE Off)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO Off)
else()
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE On)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO On)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ffunction-sections -fdata-sections")

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,--gc-sections")
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} -Wl,--gc-sections")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -Wl,--gc-sections")
#set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS_RELEASE} ")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address --param asan-stack=0 -fno-omit-frame-pointer")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address --param asan-stack=0 -fno-omit-frame-pointer")

##########################################################################################
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_LIST_DIR}/source/core/integration/cmakeModules
    ${CMAKE_CURRENT_LIST_DIR}/cmakeModules)
include(dciIntegration)

set(units)
foreach(td ${CMAKE_CURRENT_LIST_DIR}/source ${CMAKE_CURRENT_LIST_DIR}/source/core ${CMAKE_CURRENT_LIST_DIR}/source/module $ENV{DCI_EXTARNAL_UNIT_DIRS})
    file(GLOB tu ${td}/*/CMakeLists.txt)
    list(APPEND units ${tu})
endforeach()

dciIntegration(
    UNIT_SCRIPTS            ${units}
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
#    VENDOR                  $ENV{DCI_VENDOR}
#    VENDOR_KEY              $ENV{DCI_VENDOR_KEY}

#    BUILD_TESTS             On
#    STRIP_DEBUG             Off
#    SEPARATE_DEBUG          Off
)
