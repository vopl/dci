include(dciIntegrationUnit)
dciIntegrationUnit(primitives
        INCLUDE_DIRS include)

##############################################################
file(GLOB_RECURSE INC include/*)

add_library(${UNAME} STATIC ${INC})
dciIntegrationSetupTarget(${UNAME})
set_target_properties(${UNAME} PROPERTIES LINKER_LANGUAGE CXX)
