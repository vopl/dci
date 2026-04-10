include(dciIntegrationUnit)
dciIntegrationUnit(module-ppn-service-slave
        IDL_DIRS idl
        DEPENDS host)

############################################################
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
    SOURCES ppn/service/slave.idl
    NAME ppn/service/slave
)

dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES ppn/service/slave.idl
    OPTIONS --cpp-no-entities --cpp-stiac-support --cpp-stiac-support-filter "^::dci::idl::gen::ppn::service::(s|S)lave"
    NAME stiac-support
)

##############################################################
include(dciUtilsPch)
dciUtilsPch(${UNAME} src/pch.hpp ${SRC})

############################################################
include(dciHostModule)
dciHostModule(${UNAME})
