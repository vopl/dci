include(dciIntegrationUnit)
dciIntegrationUnit(module-ppn-connectivity-demand
        IDL_DIRS idl
        DEPENDS host)


file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE IDL idl/*)

add_library(${UNAME} MODULE ${INC} ${SRC} ${IDL})
target_include_directories(${UNAME} PRIVATE src)

target_link_libraries(${UNAME} PRIVATE crypto config)

##############################################################
include(dciIdl)
dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES ppn/connectivity/demand.idl
    NAME ppn/connectivity/demand
)

dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES ppn/connectivity/demand.idl
    OPTIONS --cpp-no-entities --cpp-stiac-support --cpp-stiac-support-filter "^::dci::idl::gen::ppn::connectivity::(D|d)emand"
    NAME stiac-support
)

##############################################################
include(dciUtilsPch)
dciUtilsPch(${UNAME} src/pch.hpp ${SRC})

############################################################
include(dciHostModule)
dciHostModule(${UNAME})
