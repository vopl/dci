#logarithmic identifier space

include(dciIntegrationUnit)
dciIntegrationUnit(module-ppn-topology-lis
        IDL_DIRS idl
        DEPENDS host)


file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE IDL idl/*)
file(GLOB_RECURSE TST test/*)

add_library(${UNAME} MODULE ${INC} ${SRC} ${IDL})
target_include_directories(${UNAME} PRIVATE src)
target_link_libraries(${UNAME} PRIVATE config)

##############################################################
include(dciIdl)
dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES ppn/topology/lis.idl ppn/connectivity/reest.idl ppn/connectivity/demand.idl ppn/node/rdb.idl
    NAME ppn/topology/lis
)

dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES ppn/topology/lis.idl
    OPTIONS --cpp-no-entities --cpp-stiac-support --cpp-stiac-support-filter "^::dci::idl::gen::ppn::topology::(L|l)is"
    NAME stiac-support
)

##############################################################
include(dciUtilsPch)
dciUtilsPch(${UNAME} src/pch.hpp ${SRC})

############################################################
include(dciHostModule)
dciHostModule(${UNAME})

##############################################################
dciTest(${UNAME} noenv
    SRC
        ${TST}
    LINK
        host-lib
        crypto
        exception
        utils
    DEPENDS
        ${UNAME}
)
