include(dciIntegrationUnit)
dciIntegrationUnit(idl
        INCLUDE_DIRS include
        CMM_DIRS cmakeModules
        DEPENDS cmt crypto bytes)

############################################################
file(GLOB_RECURSE INC include/*)
file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE CMM cmakeModules/* )
add_library(${UNAME} SHARED ${INC} ${SRC} ${CMM})
dciIntegrationSetupTarget(${UNAME})
target_link_libraries(${UNAME} PRIVATE
    utils)

############################################################
include(dciHimpl)
dciHimplMakeLayouts(${UNAME}
    INCLUDE
        src

    OUTPUT
        dci/idl/implMetaInfo.hpp

    HEADERS
        contract/impl/lidRegistry.hpp

    CLASSES
        dci::idl::contract::impl::LidRegistry
)

############################################################
file(GLOB_RECURSE PRS prs/*)
add_library(${UNAME}-prs STATIC ${PRS})
dciIntegrationSetupTarget(${UNAME}-prs AUX)
target_include_directories(${UNAME}-prs PRIVATE .)

file(GLOB_RECURSE IM im/*)
add_library(${UNAME}-im STATIC ${IM})
dciIntegrationSetupTarget(${UNAME}-im AUX)
target_include_directories(${UNAME}-im PRIVATE .)
add_dependencies(${UNAME}-im crypto)

file(GLOB_RECURSE GEN gen/*)
add_library(${UNAME}-gen STATIC ${GEN})
dciIntegrationSetupTarget(${UNAME}-gen AUX)
target_include_directories(${UNAME}-gen PRIVATE .)

############################################################
file(GLOB_RECURSE TRANSPILER transpiler/* )
add_executable(${UNAME}-transpiler ${TRANSPILER})
dciIntegrationSetupTarget(${UNAME}-transpiler BDEP)

target_include_directories(${UNAME}-transpiler PRIVATE .)
target_link_libraries(${UNAME}-transpiler PRIVATE
    ${UNAME}-prs
    ${UNAME}-im
    ${UNAME}-gen
    crypto
    utils
    integration
)

find_package(Boost REQUIRED COMPONENTS program_options CONFIG)
target_link_libraries(${UNAME}-transpiler PRIVATE Boost::program_options)

############################################################
file(GLOB_RECURSE TST test/* )
include(dciTest)
dciTest(${UNAME} noenv
    INCLUDE
        .
    SRC
        ${TST}
    DEFINITIONS
        -DTESTDIR="../test/${UNAME}-test-noenv"
    LINK
        ${UNAME}-prs
        ${UNAME}-im
        ${UNAME}-gen
        ${UNAME}
        crypto
        utils
        bytes
        mm
        exception
        sbs
        cmt
)

############################################################
if(TARGET ${UNAME}-test-noenv)

    include(dciIdl)
    dciIdl(${UNAME}-test-noenv cpp
        INCLUDE ${DCI_IDL_DIRS} test
        SOURCES interface/cast.idl
        NAME interface/cast
    )

    dciIdl(${UNAME}-test-noenv cpp
        INCLUDE ${DCI_IDL_DIRS} test
        SOURCES interface/call.idl
        NAME interface/call
    )

    dciIdl(${UNAME}-test-noenv cpp
        INCLUDE ${DCI_IDL_DIRS} test
        SOURCES interface/inheritance.idl
        NAME interface/inheritance
    )

    set(rsrc ${CMAKE_CURRENT_SOURCE_DIR}/test)
    set(rdstLocal test/${UNAME}-test-noenv)
    set(rdst ${DCI_OUT_DIR}/${rdstLocal})
    file(GLOB_RECURSE resources ${rsrc}/*.idl )
    foreach(resource ${resources})
        file(RELATIVE_PATH fname ${rsrc} ${resource})
        file(RELATIVE_PATH path4Comment ${CMAKE_BINARY_DIR} ${rdst}/${fname})
        add_custom_command(OUTPUT ${rdst}/${fname}
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different ${rsrc}/${fname} ${rdst}/${fname}
                           DEPENDS ${rsrc}/${fname}
                           COMMENT "Generating ${path4Comment}"
        )
        target_sources(${UNAME}-test-noenv PRIVATE ${rdst}/${fname})

        dciIntegrationMeta(UNIT ${UNAME} TARGET ${UNAME}-test-noenv RESOURCE_FILE ${rdst}/${fname} ${rdstLocal}/${fname})
    endforeach()
endif()
