
if(NOT COMMAND dciTest)
    function(dciTest uname stage)

        if(DCI_BUILD_TESTS)
            include(CMakeParseArguments)
            cmake_parse_arguments(OPTS "" "" "SRC;INCLUDE;DEFINITIONS;LINK;DEPENDS" ${ARGN})

            set(availableStages noenv mnone mstart)
            list(FIND availableStages ${stage} stageIdx)
            if(${stageIdx} EQUAL -1)
                message(FATAL_ERROR "stage '${stage}' unrecognized. Available stages are: ${availableStages}.")
            endif()

            set(testTarget ${uname}-test-${stage})
            add_library(${testTarget} SHARED ${OPTS_SRC})
            dciIntegrationSetupTarget(${testTarget} TEST)

            target_include_directories(${testTarget} SYSTEM PRIVATE ${OPTS_INCLUDE})
            target_compile_definitions(${testTarget} PRIVATE ${OPTS_DEFINITIONS})

            target_link_libraries(${testTarget} PRIVATE test ${OPTS_LINK})
            if(OPTS_DEPENDS)
                add_dependencies(${testTarget} ${OPTS_DEPENDS})
            endif()
        endif()

    endfunction()
endif()
