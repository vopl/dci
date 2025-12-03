
function(dciHostModule uname)

    include(CMakeParseArguments)
    cmake_parse_arguments(OPTS "" "" "IMPLTARGETS" ${ARGN})

    if(OPTS_IMPLTARGETS)
        message(FATAL_ERROR "not implemented yet")
    endif()

    string(REGEX REPLACE "^module-" "" mname ${uname})
    set(target ${uname})

    ######################
    dciIntegrationSetupTarget(${target} MODULE)
    target_link_libraries(${target} PRIVATE
        host-lib
        stiac
        idl
        bytes
        sbs
        exception
        mm
        cmt
        poll
        utils
        logger)
    add_dependencies(${target} host)

    get_target_property(outDir ${target} LIBRARY_OUTPUT_DIRECTORY)
    set(manifest ${outDir}/${mname}.manifest)

    file(RELATIVE_PATH path4Comment ${CMAKE_BINARY_DIR} ${manifest})
    add_custom_command(OUTPUT ${manifest}
        COMMAND host-cmd --genmanifest $<TARGET_FILE:${target}> --outfile ${manifest}
        DEPENDS ${target} ${OPTS_IMPLTARGETS} host host-cmd
        COMMENT "Generating ${path4Comment}")

    add_custom_target(${target}-manifest ALL SOURCES ${manifest})

    #target_sources(${target} PRIVATE ${manifest})

    set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_NAME "${mname}")
    #set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_NAME "main")
    set_target_properties(${target} PROPERTIES PREFIX "")

    target_compile_definitions(${target} PRIVATE -DdciModuleName="${mname}")

    include(dciIntegrationMeta)

    file(RELATIVE_PATH MANIFEST_PROJECTION ${DCI_OUT_DIR} ${manifest})
    dciIntegrationMeta(UNIT ${uname} TARGET ${uname} RESOURCE_FILE ${manifest} ${MANIFEST_PROJECTION})
    dciIntegrationMeta(DEPEND ${target}-manifest)
endfunction()
