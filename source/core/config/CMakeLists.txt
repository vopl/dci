include(dciIntegrationUnit)
dciIntegrationUnit(config
        INCLUDE_DIRS include
        IDL_DIRS idl
        DEPENDS idl logger utils
)

##############################################################
file(GLOB_RECURSE INC include/*)
file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE IDL idl/*)

add_library(${UNAME} SHARED ${INC} ${SRC} ${IDL})
dciIntegrationSetupTarget(${UNAME})
target_include_directories(${UNAME} PRIVATE src)

target_link_libraries(${UNAME} PRIVATE logger utils)

##############################################################
include(dciIdl)
dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES config.idl
    NAME idl-config
)
