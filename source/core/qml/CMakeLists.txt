return()

include(dciIntegrationUnit)
dciIntegrationUnit(qml
        INCLUDE_DIRS include
        DEPENDS poll idl primitives)

############################################################
file(GLOB_RECURSE INC include/*)
file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE TST test/*)

add_library(${UNAME} SHARED ${INC} ${SRC})
dciIntegrationSetupTarget(${UNAME})
target_include_directories(${UNAME} PRIVATE src)

include(dciUtilsPch)
dciUtilsPch(${UNAME} src/pch.hpp ${SRC})

target_link_libraries(${UNAME} PRIVATE
    exception
    mm
    cmt
    sbs
    utils
    poll
    logger
    Qt6::Quick
    Qt6::Widgets)


if(WIN32)
    set(needQtComponents)
else()
    set(needQtComponents XcbQpaPrivate)
    target_link_libraries(${UNAME} PRIVATE Qt6::XcbQpaPrivate)
endif()

find_package(Qt6 COMPONENTS Core Quick Widgets ${needQtComponents} REQUIRED)
get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)

############################################################
include(dciHimpl)
dciHimplMakeLayouts(${UNAME}
    INCLUDE
        src
        ${Qt6Quick_INCLUDE_DIRS}
        ${Qt6Quick_PRIVATE_INCLUDE_DIRS}

    OUTPUT
        dci/qml/implMetaInfo.hpp

    HEADERS
        impl/app.hpp

    CLASSES
        dci::qml::impl::App

    LINK
        Qt6::Quick
        Qt6::Widgets
)

target_include_directories(${UNAME} SYSTEM PRIVATE
    ${Qt6Quick_INCLUDE_DIRS}
    ${Qt6Quick_PRIVATE_INCLUDE_DIRS}
    ${Qt6Widgets_INCLUDE_DIRS}
    ${Qt6Widgets_PRIVATE_INCLUDE_DIRS})
target_link_libraries(${UNAME} PRIVATE Qt6::Quick)

set_target_properties(${UNAME} PROPERTIES AUTOMOC On)
set_target_properties(${UNAME} PROPERTIES AUTORCC On)
set_target_properties(${UNAME} PROPERTIES AUTOUIC On)

############################################################
dciIntegrationMeta(UNIT ${UNAME} TARGET ${UNAME} RESOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/resources/qt.conf bin/qt.conf)

############################################################
include(ExternalProject)
ExternalProject_Add(${UNAME}-open-sans
    EXCLUDE_FROM_ALL On
    DOWNLOAD_EXTRACT_TIMESTAMP On
    #URL http://www.opensans.com/download/open-sans.zip
    #URL_HASH SHA256=36bf1f3d470840208e0e617a6bad793111c7f535cd4339e07618aaf4c179fd0c

    #URL https://dev.gentoo.org/~jstein/dist/open-sans-1.zip
    #URL_HASH SHA256=10ebb03bbca8309e622d46b7b94645b64a124cc84c1f7c2e112a3d0b4fad12b1

    #URL https://jamaica4hclubs.com/images/Fonts/open-sans.zip
    #URL_HASH SHA256=92125e92e582fe84807ac680edb369acb8fd64b7c242119834c2ad08ab847b9b

    URL https://www.1001fonts.com/download/open-sans.zip
    URL_HASH SHA256=956967e70836627cd1df713480484078dedd45a338218b902002f348c1c44358

    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/open-sans-src
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/open-sans
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/open-sans-src ${CMAKE_CURRENT_BINARY_DIR}/open-sans
            COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_CURRENT_BINARY_DIR}/open-sans/Apache License.txt"
            COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_CURRENT_BINARY_DIR}/open-sans/LICENSE.txt"
)
dciIntegrationMeta(DEPEND ${UNAME}-open-sans)
dciIntegrationMeta(UNIT ${UNAME} TARGET ${UNAME} RESOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/open-sans lib/fonts)

############################################################
function(addResourceFiles srcPrefix dstPrefix)
    dciIntegrationMeta(UNIT ${UNAME} DIR_MAPPING ${srcPrefix} ${dstPrefix})
    foreach(abs ${ARGN})
        file(RELATIVE_PATH rel ${srcPrefix} ${abs})
        dciIntegrationMeta(UNIT ${UNAME} TARGET ${UNAME} RESOURCE_FILE ${abs} ${dstPrefix}/${rel})
        if(abs MATCHES "\\.so")
            dciIntegrationMeta(UNIT ${UNAME} TARGET ${UNAME} FILE_FOR_TARGET_DEPS ${abs})
        endif()
    endforeach()
endfunction()

############################################################
execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_PLUGINS        OUTPUT_VARIABLE QT_INSTALL_PLUGINS      OUTPUT_STRIP_TRAILING_WHITESPACE)
file(GLOB_RECURSE PLUG_FILES
    ${QT_INSTALL_PLUGINS}/audio/*
    ${QT_INSTALL_PLUGINS}/iconengines/*
    ${QT_INSTALL_PLUGINS}/imageformats/*
    ${QT_INSTALL_PLUGINS}/platforminputcontexts/*
    ${QT_INSTALL_PLUGINS}/platforms/*
    ${QT_INSTALL_PLUGINS}/platformthemes/*
    ${QT_INSTALL_PLUGINS}/printsupport/*
    #${QT_INSTALL_PLUGINS}/qmltooling/*
    ${QT_INSTALL_PLUGINS}/sqldrivers/*
    ${QT_INSTALL_PLUGINS}/xcbglintegrations/*)
list(FILTER PLUG_FILES EXCLUDE REGEX ".*\\.debug$")
addResourceFiles(${QT_INSTALL_PLUGINS} lib/qt6/plugins ${PLUG_FILES})

############################################################
execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_QML            OUTPUT_VARIABLE QT_INSTALL_QML          OUTPUT_STRIP_TRAILING_WHITESPACE)
file(GLOB_RECURSE QML_FILES
    ${QT_INSTALL_QML}/Qt/*
    ${QT_INSTALL_QML}/QtGraphicalEffects/*
    #${QT_INSTALL_QML}/QtLocation/*
    ${QT_INSTALL_QML}/QtMultimedia/*
    #${QT_INSTALL_QML}/QtPositioning/*
    ${QT_INSTALL_QML}/QtQml/*
    ${QT_INSTALL_QML}/QtQuick/*
    ${QT_INSTALL_QML}/QtQuick.2/*
    #${QT_INSTALL_QML}/QtTest/*
    #${QT_INSTALL_QML}/QtWebChannel/*
    #${QT_INSTALL_QML}/QtWebEngine/*
    #${QT_INSTALL_QML}/QtWebKit/*
    ${QT_INSTALL_QML}/builtins.qmltypes)
list(FILTER QML_FILES EXCLUDE REGEX ".*\\.debug$")
list(FILTER QML_FILES EXCLUDE REGEX ".*\\.qmlc$")
addResourceFiles(${QT_INSTALL_QML} lib/qt6/qml ${QML_FILES})

############################################################
include(dciTest)
dciTest(${UNAME} mnone
    SRC ${TST}
    INCLUDE
        ${Qt6Core_PRIVATE_INCLUDE_DIRS}
        ${Qt6Quick_INCLUDE_DIRS}
        ${Qt6Quick_PRIVATE_INCLUDE_DIRS}
        ${Qt6Widgets_INCLUDE_DIRS}
        ${Qt6Widgets_PRIVATE_INCLUDE_DIRS}
    LINK
        ${UNAME}
        exception
        cmt
        sbs
        mm
        idl
        Qt6::Quick
)

if(TARGET ${UNAME}-test-mnone)
    dciIdl(${UNAME}-test-mnone cpp
        INCLUDE test
        SOURCES victim.idl
        NAME idl-victim
    )
endif()
