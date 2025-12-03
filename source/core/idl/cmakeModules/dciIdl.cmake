
function(dciIdl target gen)

    include(CMakeParseArguments)
    cmake_parse_arguments(OPTS "" "NAME;OUTPUT;ARTEFACTS" "OPTIONS;SOURCES;INCLUDE" ${ARGN})

    if(OPTS_NAME)
        set(NAME ${OPTS_NAME})
    else()
        list(GET OPTS_SOURCES 0 NAME)
        if(NOT NAME)
            set(NAME generated)
        endif()
        get_filename_component(NAME ${NAME} NAME_WE)
    endif()

    set(outDir ${CMAKE_CURRENT_BINARY_DIR}/idl-generated)

    if(OPTS_OUTPUT)
        get_filename_component(outFile ${outDir}/${OPTS_OUTPUT} ABSOLUTE)
    else()
        set(outFile ${outDir}/${NAME}.hpp)
    endif()

    if(OPTS_ARTEFACTS)
        set(${OPTS_ARTEFACTS} ${outFile} PARENT_SCOPE)
    endif()


    file(MAKE_DIRECTORY ${outDir})
    target_include_directories(${target} PRIVATE ${outDir})

    set_source_files_properties(${outFile} PROPERTIES GENERATED TRUE)

    foreach(i ${OPTS_INCLUDE})
        get_filename_component(i2 ${i} REALPATH)
        if(IS_DIRECTORY ${i2})
            set(include ${include} --include ${i2})
        else()
            message(FATAL_ERROR "Bad directory for idl inclusion: ${i}, source dir: ${CMAKE_CURRENT_SOURCE_DIR}, target: ${target}")
        endif()
    endforeach()

    set(sources)
    foreach(s ${OPTS_SOURCES})
        set(sources ${sources} --in ${s})
    endforeach()

    set(options)
    foreach(o ${OPTS_OPTIONS})
        set(options ${options} ${o})
    endforeach()

    if("${CMAKE_GENERATOR}" STREQUAL "Ninja")
        # for Ninja
        set(outFileDeps ${outDir}/${NAME}.d)
        file(RELATIVE_PATH path4Comment ${CMAKE_BINARY_DIR} ${outFile})
        add_custom_command(OUTPUT ${outFile}
            COMMAND idl-transpiler-cmd ${include} ${sources} --generate ${gen} ${options} --${gen}-output-file ${outFile} --generate deps --deps-target ninja --deps-ninja-artifact ${outFile} --deps-output-file ${outFileDeps}
            VERBATIM
            DEPENDS idl-transpiler idl-transpiler-cmd
            DEPFILE ${outFileDeps}
            COMMENT "Generating ${path4Comment}"
        )
    else()
        # for Makefiles
        set(outFileDeps ${outDir}/${NAME}.hpp.d.hpp)
        file(RELATIVE_PATH path4Comment ${CMAKE_BINARY_DIR} ${outFileDeps})
        add_custom_command(OUTPUT ${outFileDeps}
            COMMAND idl-transpiler-cmd ${include} ${sources} --generate deps --deps-target cpp --deps-output-file ${outFileDeps}
            VERBATIM
            DEPENDS idl-transpiler idl-transpiler-cmd
            COMMENT "Generating ${path4Comment}"
        )
        file(RELATIVE_PATH path4Comment ${CMAKE_BINARY_DIR} ${outFile})
        add_custom_command(OUTPUT ${outFile}
            COMMAND idl-transpiler-cmd ${include} ${sources} --generate ${gen} ${options} --${gen}-output-file ${outFile}
            VERBATIM
            DEPENDS idl-transpiler idl-transpiler-cmd ${outFileDeps}
            COMMENT "Generating ${path4Comment}"
            IMPLICIT_DEPENDS CXX ${outFileDeps}
        )
    endif()

    target_sources(${target} PRIVATE ${outFile})

endfunction()
