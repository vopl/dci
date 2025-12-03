include(dciIntegrationUnit)
dciIntegrationUnit(logger
        INCLUDE_DIRS include
        DEPENDS utils)

file(GLOB_RECURSE INC include/*)
file(GLOB_RECURSE SRC src/*)

add_library(${UNAME} SHARED ${INC} ${SRC})
target_link_libraries(${UNAME} PRIVATE utils)
dciIntegrationSetupTarget(${UNAME})
