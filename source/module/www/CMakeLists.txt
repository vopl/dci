include(dciIntegrationUnit)
dciIntegrationUnit(module-www
        IDL_DIRS idl
        DEPENDS host stiac crypto utils)

file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE IDL idl/*)
file(GLOB_RECURSE TST test/*)

add_library(${UNAME} MODULE ${SRC} ${IDL})
target_include_directories(${UNAME} PRIVATE src)

############################################################
find_package(ZLIB REQUIRED)
target_link_libraries(${UNAME} PRIVATE
    ZLIB::ZLIB
    zstd
    brotlienc
    brotlidec
    stiac
    crypto
    utils
    -lssl -lcrypto
    )

##############################################################
include(dciIdl)
dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES www.idl net.idl
    ARTEFACTS WWW_IDL_ARTEFACTS
)

dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES www.idl
    OPTIONS --cpp-no-entities --cpp-stiac-support --cpp-stiac-support-filter "^::dci::idl::gen::www::"
    NAME www-stiac-support
)

##############################################################
add_executable(${UNAME}-enumSupport-gen aux_/enumSupport-gen.cpp ${WWW_IDL_ARTEFACTS})
dciIntegrationSetupTarget(${UNAME}-enumSupport-gen AUX)
foreach(wwwIdlArtefact ${WWW_IDL_ARTEFACTS})
    get_filename_component(wwwIdlArtefactDir ${wwwIdlArtefact} DIRECTORY)
    target_include_directories(${UNAME}-enumSupport-gen PRIVATE ${wwwIdlArtefactDir})
endforeach()
target_include_directories(${UNAME}-enumSupport-gen PRIVATE src)
target_link_libraries(${UNAME}-enumSupport-gen PRIVATE
    exception
    idl
    sbs
    mm
    stiac
    crypto
)

add_custom_command(OUTPUT enumSupport.cpp
                   COMMAND ${UNAME}-enumSupport-gen-cmd > enumSupport.cpp
                   DEPENDS ${UNAME}-enumSupport-gen ${UNAME}-enumSupport-gen-cmd
)

target_sources(${UNAME} PRIVATE enumSupport.cpp)


##############################################################
include(dciUtilsPch)
dciUtilsPch(${UNAME} src/pch.hpp ${SRC})

############################################################
include(dciHostModule)
dciHostModule(${UNAME})

##############################################################
dciTest(${UNAME} mstart
    SRC
        ${TST}
    LINK
        host-lib
        bytes
        sbs
        exception
        mm
        cmt
        poll
        idl
        logger
    DEPENDS
        ${UNAME}
)

if(TARGET ${UNAME}-test-mstart)
    dciIdl(${UNAME}-test-mstart cpp
        INCLUDE ${DCI_IDL_DIRS}
        NAME www4test
        SOURCES www.idl net.idl
    )
endif()
