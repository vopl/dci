include(dciIntegrationUnit)
dciIntegrationUnit(module-ppn-node
        IDL_DIRS idl
        DEPENDS host)

file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE IDL idl/*)
file(GLOB_RECURSE TST test/*)

add_library(${UNAME} MODULE ${INC} ${SRC} ${IDL})
target_include_directories(${UNAME} PRIVATE src)
target_link_libraries(${UNAME} PRIVATE crypto config)

if(WIN32)
    target_link_libraries(${UNAME} PRIVATE Iphlpapi.lib)
endif()

##############################################################
include(dciIdl)
dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES
        host/daemon.idl
        configurable.idl
        ppn/node.idl
        ppn/node/rdb.idl
        ppn/node/link.idl
        ppn/transport.idl
        ppn/transport/inproc.idl
        ppn/transport/net.idl
        ppn/transport/natt.idl
        net.idl
    NAME
        ppn/node
)

dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES ppn/node.idl
    OPTIONS --cpp-no-entities --cpp-stiac-support --cpp-stiac-support-filter "^::dci::idl::gen::ppn::(Node|node::(Feature|feature::))"
    NAME stiac-support
)

##############################################################
include(dciUtilsPch)
dciUtilsPch(${UNAME} src/pch.hpp ${SRC})

############################################################
include(dciHostModule)
dciHostModule(${UNAME})

############################################################
set(conf etc/ppn-node.conf.example)
dciIntegrationMeta(UNIT ${UNAME} RESOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${conf} ${conf})
add_custom_command(
    OUTPUT ${DCI_OUT_DIR}/${conf}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${conf} ${DCI_OUT_DIR}/${conf}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${conf}
    COMMENT "Copying ${DCI_OUT_DIR}/${conf}")
target_sources(${UNAME} PRIVATE ${DCI_OUT_DIR}/${conf})

##############################################################
dciTest(${UNAME} mstart
    SRC
        ${TST}
    LINK
        sbs
        exception
        mm
        idl
    DEPENDS
        ${UNAME}
)

if(TARGET ${UNAME}-test-mstart)
    dciIdl(${UNAME}-test-mstart cpp
        INCLUDE ${DCI_IDL_DIRS}
        SOURCES
            host/daemon.idl
            configurable.idl
            ppn/node.idl
            ppn/node/rdb.idl
            ppn/node/link.idl
            ppn/transport.idl
            ppn/transport/inproc.idl
            ppn/transport/net.idl
            ppn/transport/natt.idl
            net.idl
        NAME
            ppn/node
    )
endif()
