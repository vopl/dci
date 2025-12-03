include(dciIntegrationUnit)
dciIntegrationUnit(module-stiac
        IDL_DIRS idl
        DEPENDS host)

############################################################
file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE IDL idl/*)
file(GLOB_RECURSE TST test/*)

add_library(${UNAME} MODULE ${INC} ${SRC} ${IDL})
target_include_directories(${UNAME} PRIVATE src)

target_link_libraries(${UNAME} PRIVATE
    crypto
    zstd)

##############################################################
include(dciIdl)
dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES stiac.idl
    NAME stiac
)

dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES stiac.idl
    OPTIONS --cpp-no-entities --cpp-stiac-support --cpp-stiac-support-filter "^::dci::idl::gen::stiac::"
    NAME stiac-stiac-support
)

##############################################################
include(dciUtilsPch)
dciUtilsPch(${UNAME} src/pch.hpp ${SRC})

############################################################
include(dciHostModule)
dciHostModule(${UNAME})

##############################################################
include(dciTest)
dciTest(${UNAME} mstart
    SRC
        ${TST}
    LINK
        host-lib
        bytes
        sbs
        utils
        exception
        mm
        cmt
        stiac
        idl
    DEPENDS
        ${UNAME}
)

##############################################################
if(TARGET ${UNAME}-test-mstart)
    dciIdl(${UNAME}-test-mstart cpp
        INCLUDE .
        SOURCES test/victimInterface.idl
        OPTIONS --cpp-stiac-support --cpp-stiac-support-filter "^::dci::idl::gen::stiac::test::"
        NAME test/victimInterface
    )
    dciIdl(${UNAME}-test-mstart cpp
        INCLUDE ${DCI_IDL_DIRS}
        SOURCES stiac.idl
        NAME stiac
    )
endif()
