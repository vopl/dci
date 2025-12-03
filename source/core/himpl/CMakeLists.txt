include(dciIntegrationUnit)
dciIntegrationUnit(himpl
        INCLUDE_DIRS include
        CMM_DIRS cmakeModules)

file(GLOB_RECURSE INC include/*)
file(GLOB_RECURSE CMM cmakeModules/*)

add_library(${UNAME} STATIC ${INC} ${CMM})
dciIntegrationSetupTarget(${UNAME})
set_target_properties(${UNAME} PROPERTIES LINKER_LANGUAGE CXX)
