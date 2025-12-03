if(NOT COMMAND dciIntegrationMeta)

    set(dciIntegrationMeta_dir ${CMAKE_CURRENT_LIST_DIR})

    ##############################################################
    function(dciIntegrationMeta)

        set(payloadArgs
                DIR_MAPPING
                SYSLIB_MAPTO
                SYSLIB_IGNORE

                SRC_FILE
                SRC_DIR
                CMM_FILE
                CMM_DIR
                INCLUDE_FILE
                INCLUDE_DIR
                IDL_FILE
                IDL_DIR
                RESOURCE_FILE
                RESOURCE_DIR
                EXTRA_ALLOWED

                TARGET_TYPE
                TARGET_KIND
                TARGET_FILE
                TARGET_DEPS)

        set(options)
        set(oneValueArgs UNIT TARGET)
        set(multiValueArgs COMMAND DEPEND FILE_FOR_TARGET_DEPS ${payloadArgs})
        cmake_parse_arguments(A "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

        foreach(file ${A_FILE_FOR_TARGET_DEPS})
            #list(APPEND A_COMMAND "${CMAKE_COMMAND} -D u=\"${A_UNIT}\" -D t=\"${A_TARGET}\" -D f=\"${file}\" -D libdir=\"${DCI_OUT_DIR}/lib\" -P ${dciIntegrationMeta_dir}/dciIntegrationMetaFetchTargetRuntimeDeps.script")
            list(APPEND A_COMMAND "sh ${dciIntegrationMeta_dir}/dciIntegrationMetaFetchTargetRuntimeDeps.sh \"${A_UNIT}\" \"${A_TARGET}\" \"${file}\" \"${DCI_OUT_DIR}/lib\"")
        endforeach()

        set(unitPart)
        if(A_UNIT)
            set(unitPart "UNIT[${A_UNIT}]")
        endif()

        set(targetPart)
        if(A_TARGET)
            set(targetPart "TARGET[${A_TARGET}]")
        endif()

        foreach(pa ${payloadArgs})
            if(A_${pa})
                set(tmp ${unitPart} ${targetPart} ${pa}[${A_${pa}}])
                list(JOIN tmp " " tmp)
                list(APPEND A_COMMAND "echo \"${tmp}\"")
            endif()
        endforeach()

        get_property(commands GLOBAL PROPERTY dciIntegrationMeta_commands)
        foreach(c ${A_COMMAND})
            string(SHA256 hash "${A_UNIT};${A_TARGET};${c}")
            set(cname dciIntegrationMetaCommand_${hash})

            if(NOT ${cname} IN_LIST commands)
                set_property(GLOBAL PROPERTY ${cname} "${c}")
                list(APPEND commands ${cname})
            endif()
        endforeach()
        set_property(GLOBAL PROPERTY dciIntegrationMeta_commands ${commands})

        get_property(deps GLOBAL PROPERTY dciIntegrationMeta_deps)
        list(APPEND deps ${A_DEPEND} ${A_TARGET})
        list(REMOVE_DUPLICATES deps)
        set_property(GLOBAL PROPERTY dciIntegrationMeta_deps ${deps})
    endfunction()

endif()
