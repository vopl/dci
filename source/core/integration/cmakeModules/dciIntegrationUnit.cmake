if(NOT COMMAND dciIntegrationUnit)

    ############################################################################
    include(${CMAKE_CURRENT_LIST_DIR}/dciIntegrationMeta.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/dciIntegrationSetupTarget.cmake)

    ############################################################################
    function(dciIntegrationUnitAbsolutizeDirectories base list)
        set(results)
        foreach(one ${${list}})
            set(result)
            if(IS_DIRECTORY ${base}/${one})
                set(result ${base}/${one})
            elseif(IS_DIRECTORY ${one})
                set(result ${one})
            else()
                message(FATAL_ERROR "Unable to resolve directory: [${one}] with base [${base}]")
            endif()

            get_filename_component(result ${result} REALPATH)
            file(TO_CMAKE_PATH ${result} result)
            list(APPEND results ${result})
        endforeach()
        set(${list} ${results} PARENT_SCOPE)
    endfunction()

    ############################################################################
    function(dciIntegrationUnitAccumuleDepends resultDependsList uname)
        set(res)
        get_property(deps GLOBAL PROPERTY DCI_INTEGRATION_REGISTRY_${uname}_DEPENDS)
        foreach(dep ${deps})
            dciIntegrationUnitAccumuleDepends(res2 ${dep})
            list(APPEND res ${res2} ${dep})
        endforeach()
        list(REMOVE_DUPLICATES res)
        set(${resultDependsList} ${res} PARENT_SCOPE)
    endfunction()


    ############################################################################
    macro(dciIntegrationUnit name)
        set(options WANT_POSTPROCESSING)
        set(oneValueArgs)
        set(multiValueArgs DEPENDS CMM_DIRS INCLUDE_DIRS IDL_DIRS)
        cmake_parse_arguments(A "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

        if(NOT ("integration" STREQUAL ${name}))
            set(A_DEPENDS integration ${A_DEPENDS})
        endif()

        dciIntegrationUnitAbsolutizeDirectories(${CMAKE_CURRENT_LIST_DIR} A_CMM_DIRS)
        dciIntegrationUnitAbsolutizeDirectories(${CMAKE_CURRENT_LIST_DIR} A_INCLUDE_DIRS)
        dciIntegrationUnitAbsolutizeDirectories(${CMAKE_CURRENT_LIST_DIR} A_IDL_DIRS)

        list(REMOVE_DUPLICATES A_DEPENDS)
        list(REMOVE_DUPLICATES A_CMM_DIRS)
        list(REMOVE_DUPLICATES A_INCLUDE_DIRS)
        list(REMOVE_DUPLICATES A_IDL_DIRS)

        if(dciIntegrationStage1)

            file(RELATIVE_PATH SRC_DIR_PROJECTION ${DCI_SRC_DIR} ${CMAKE_CURRENT_LIST_DIR})
            dciIntegrationMeta(UNIT ${name} SRC_DIR ${CMAKE_CURRENT_LIST_DIR} ${SRC_DIR_PROJECTION})

            foreach(DIR ${A_CMM_DIRS})
                get_filename_component(ADIR ${DIR} ABSOLUTE BASE_DIR ${DCI_SRC_DIR})
                dciIntegrationMeta(UNIT ${name} CMM_DIR ${ADIR} cmakeModules)
            endforeach()

            foreach(DIR ${A_INCLUDE_DIRS})
                get_filename_component(ADIR ${DIR} ABSOLUTE BASE_DIR ${DCI_SRC_DIR})
                dciIntegrationMeta(UNIT ${name} INCLUDE_DIR ${ADIR} include)
            endforeach()

            foreach(DIR ${A_IDL_DIRS})
                get_filename_component(ADIR ${DIR} ABSOLUTE BASE_DIR ${DCI_SRC_DIR})
                dciIntegrationMeta(UNIT ${name} IDL_DIR ${ADIR} idl)
            endforeach()

            get_property(units GLOBAL PROPERTY DCI_INTEGRATION_REGISTRY_UNITS)
            list(APPEND units ${name})
            list(REMOVE_DUPLICATES units)
            set_property(GLOBAL PROPERTY DCI_INTEGRATION_REGISTRY_UNITS ${units})

            set_property(GLOBAL PROPERTY DCI_INTEGRATION_REGISTRY_${name}              On)
            set_property(GLOBAL PROPERTY DCI_INTEGRATION_REGISTRY_${name}_SRC_DIR      ${CMAKE_CURRENT_LIST_DIR})
            set_property(GLOBAL PROPERTY DCI_INTEGRATION_REGISTRY_${name}_CMAKE_SCRIPT ${CMAKE_CURRENT_LIST_FILE})
            set_property(GLOBAL PROPERTY DCI_INTEGRATION_REGISTRY_${name}_DEPENDS      ${A_DEPENDS})
            set_property(GLOBAL PROPERTY DCI_INTEGRATION_REGISTRY_${name}_CMM_DIRS     ${A_CMM_DIRS})
            set_property(GLOBAL PROPERTY DCI_INTEGRATION_REGISTRY_${name}_INCLUDE_DIRS ${A_INCLUDE_DIRS})
            set_property(GLOBAL PROPERTY DCI_INTEGRATION_REGISTRY_${name}_IDL_DIRS     ${A_IDL_DIRS})

            get_property(cmmDirs GLOBAL PROPERTY DCI_CMM_DIRS)
            get_property(includeDirs GLOBAL PROPERTY DCI_INCLUDE_DIRS)
            get_property(idlDirs GLOBAL PROPERTY DCI_IDL_DIRS)

            list(APPEND cmmDirs ${A_CMM_DIRS})
            list(REMOVE_DUPLICATES cmmDirs)

            list(APPEND includeDirs ${A_INCLUDE_DIRS})
            list(REMOVE_DUPLICATES includeDirs)

            list(APPEND idlDirs ${A_IDL_DIRS})
            list(REMOVE_DUPLICATES idlDirs)

            set_property(GLOBAL PROPERTY DCI_CMM_DIRS     ${cmmDirs})
            set_property(GLOBAL PROPERTY DCI_INCLUDE_DIRS ${includeDirs})
            set_property(GLOBAL PROPERTY DCI_IDL_DIRS     ${idlDirs})

            # фейковый таргет для юнита, через него будут выстраиваться меж-юнитные зависимости для реальных таргетов
            add_custom_target(unit-${name} ALL)
            return()
        endif()

        if(dciIntegrationStage2 OR dciIntegrationStage3)

            dciIntegrationUnitAccumuleDepends(depends ${name})
            #message("---------------------- [${name}] [${A_DEPENDS}] [${depends}]")

            set(cmmDirs ${A_CMM_DIRS})
            set(includeDirs ${A_INCLUDE_DIRS})
            set(idlDirs ${A_IDL_DIRS})

            foreach(dep ${depends})
                get_property(cmmDirsByDep GLOBAL PROPERTY DCI_INTEGRATION_REGISTRY_${dep}_CMM_DIRS)
                list(APPEND cmmDirs ${cmmDirsByDep})

                get_property(includeDirsByDep GLOBAL PROPERTY DCI_INTEGRATION_REGISTRY_${dep}_INCLUDE_DIRS)
                list(APPEND includeDirs ${includeDirsByDep})

                get_property(idlDirsByDep GLOBAL PROPERTY DCI_INTEGRATION_REGISTRY_${dep}_IDL_DIRS)
                list(APPEND idlDirs ${idlDirsByDep})
            endforeach()

            list(REMOVE_DUPLICATES cmmDirs)
            list(REMOVE_DUPLICATES includeDirs)
            list(REMOVE_DUPLICATES idlDirs)

            set(DCI_UNIT_NAME ${name})
            set(UNAME ${name})
            set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${cmmDirs})
            list(REMOVE_DUPLICATES CMAKE_MODULE_PATH)
            set(DCI_INCLUDE_DIRS ${includeDirs})
            set(DCI_IDL_DIRS ${idlDirs})

            get_property(DCI_CMM_DIRS GLOBAL PROPERTY DCI_CMM_DIRS)
            get_property(DCI_INCLUDE_DIRS GLOBAL PROPERTY DCI_INCLUDE_DIRS)
            get_property(DCI_IDL_DIRS GLOBAL PROPERTY DCI_IDL_DIRS)

            if(dciIntegrationStage3 AND NOT A_WANT_POSTPROCESSING)
                return()
            endif()

            # dciIntegrationStage2 - ok, allow further execution
#            message(STATUS "DCI_UNIT_NAME: ${DCI_UNIT_NAME}")
#            message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
#            message(STATUS "DCI_INCLUDE_DIRS: ${DCI_INCLUDE_DIRS}")
#            message(STATUS "DCI_IDL_DIRS: ${DCI_IDL_DIRS}")
        endif()
    endmacro()
endif()
