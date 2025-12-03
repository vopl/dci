include(dciIntegrationUnit)
dciIntegrationUnit(exception
        INCLUDE_DIRS include
        DEPENDS utils)

file(GLOB_RECURSE INC include/*)
file(GLOB_RECURSE SRC src/*)

add_library(${UNAME} SHARED ${INC} ${SRC})
dciIntegrationSetupTarget(${UNAME})

target_link_libraries(${UNAME} utils)
