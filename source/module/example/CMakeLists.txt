include(dciIntegrationUnit)
dciIntegrationUnit(module-example
        IDL_DIRS idl
        DEPENDS host)

##############################################################
set(SRC
    src/entry.cpp
)

add_library(${UNAME} MODULE ${INC} ${SRC} idl/example.idl)
target_include_directories(${UNAME} PRIVATE src)

##############################################################
include(dciIdl)
dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    OPTIONS --cpp-stiac-support --cpp-stiac-support-filter "^::dci::idl::gen::example::"
    SOURCES example.idl
)

############################################################
include(dciHostModule)
dciHostModule(${UNAME})
