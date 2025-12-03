include(dciIntegrationUnit)
dciIntegrationUnit(module-ppn-transport-inproc
        IDL_DIRS idl
        DEPENDS host)


file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE IDL idl/*)

add_library(${UNAME} MODULE ${INC} ${SRC} ${IDL})
target_include_directories(${UNAME} PRIVATE src)

##############################################################
include(dciIdl)
dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES ppn/transport/inproc.idl
    NAME ppn/transport/inproc
)

dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES ppn/transport/inproc.idl
    OPTIONS --cpp-no-entities --cpp-stiac-support --cpp-stiac-support-filter "^::dci::idl::gen::ppn::transport::inproc::"
    NAME stiac-support
)

##############################################################
include(dciUtilsPch)
dciUtilsPch(${UNAME} src/pch.hpp ${SRC})

############################################################
include(dciHostModule)
dciHostModule(${UNAME})
