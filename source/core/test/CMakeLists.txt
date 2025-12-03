include(dciIntegrationUnit)
dciIntegrationUnit(test
        INCLUDE_DIRS include
        CMM_DIRS cmakeModules)

##############################################################
file(GLOB_RECURSE INC include/*)
file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE CMM cmakeModules/*)

############################################################
add_library(${UNAME} SHARED ${INC} ${SRC} ${CMM})
dciIntegrationSetupTarget(${UNAME})

############################################################
find_package(GTest REQUIRED)
target_link_libraries(${UNAME} PUBLIC GTest::gtest)
