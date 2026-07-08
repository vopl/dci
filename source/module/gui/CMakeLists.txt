include(dciIntegrationUnit)
dciIntegrationUnit(module-gui
        DEPENDS host qml)

############################################################
file(GLOB_RECURSE SRC src/*)
#file(GLOB_RECURSE IDL idl/*)
#file(GLOB_RECURSE TST test/*)
file(GLOB_RECURSE QML qml/*)

add_library(${UNAME} MODULE ${INC} ${SRC} ${IDL} ${QML})
target_include_directories(${UNAME} PRIVATE src)


find_package(Qt6 COMPONENTS Core Quick REQUIRED)

target_include_directories(${UNAME} SYSTEM PRIVATE
    ${Qt6Quick_INCLUDE_DIRS}
    ${Qt6Widgets_INCLUDE_DIRS})
target_link_libraries(${UNAME} PRIVATE
    qml
    crypto
    Qt6::Quick)

set_target_properties(${UNAME} PROPERTIES AUTOMOC On)
set_target_properties(${UNAME} PROPERTIES AUTORCC On)
set_target_properties(${UNAME} PROPERTIES AUTOUIC On)

##############################################################
dciIntegrationMeta(UNIT ${UNAME} RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml source/module/gui/qml)

##############################################################
include(dciIdl)
dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES host/daemon.idl
    NAME host-daemon
)

##############################################################
include(dciUtilsPch)
dciUtilsPch(${UNAME} src/pch.hpp ${SRC})

############################################################
include(dciHostModule)
dciHostModule(${UNAME})

############################################################
string(REGEX REPLACE "^module-" "" mname ${UNAME})
set(rsrc ${CMAKE_CURRENT_SOURCE_DIR})
set(rdstLocal module/${mname})
set(rdst ${DCI_OUT_DIR}/${rdstLocal})
foreach(resource ${QML})
    file(RELATIVE_PATH fname ${rsrc} ${resource})
    add_custom_command(OUTPUT ${rdst}/${fname}
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different ${rsrc}/${fname} ${rdst}/${fname}
                       DEPENDS ${rsrc}/${fname}
                       COMMENT "Generating ${rdst}/${fname}"
    )
    target_sources(${UNAME} PRIVATE ${rdst}/${fname})

    dciIntegrationMeta(UNIT ${UNAME} TARGET ${UNAME} RESOURCE_FILE ${rdst}/${fname} ${rdstLocal}/${fname})
endforeach()
