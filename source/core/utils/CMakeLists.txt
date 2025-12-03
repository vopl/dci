include(dciIntegrationUnit)
dciIntegrationUnit(utils
        INCLUDE_DIRS include
        CMM_DIRS cmakeModules
        DEPENDS test)

file(GLOB_RECURSE INC include/*)
file(GLOB_RECURSE SRC src/*)

if(NOT WIN32)
    list(FILTER SRC EXCLUDE REGEX .*win32.*)
endif()

add_library(${UNAME} SHARED ${INC} ${SRC} cmakeModules/dciUtilsPch.cmake)
dciIntegrationSetupTarget(${UNAME})

if(WIN32)
    target_link_libraries(${UNAME} PRIVATE Ws2_32.lib)
endif()

file(GLOB_RECURSE TST test/*)
include(dciTest)
dciTest(${UNAME} noenv
    SRC ${TST}
    LINK ${UNAME}
)
