
include(dciIntegrationSetupTarget)
include(dciIntegrationMeta)

if(NOT COMMAND dciHimplMakeLayouts)
    function(dciHimplMakeLayouts target)

        get_target_property(uname ${target} UNAME)
        if(NOT uname)
            message(WARN "dciHimplMakeLayouts with bad target: ${target}")
            set(uname ${target})
        endif()

        set(options )
        set(oneValueArgs OUTPUT PREDECL)
        set(multiValueArgs HEADERS CLASSES INCLUDE LINK DEPENDS)
        cmake_parse_arguments(A "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    #    message("A_OUTPUT: ${A_OUTPUT}")
    #    message("A_PREDECL: ${A_PREDECL}")
    #    message("A_HEADERS: ${A_HEADERS}")
    #    message("A_CLASSES: ${A_CLASSES}")
    #    message("A_LINK: ${A_LINK}")
    #    message("A_INCLUDE: ${A_INCLUDE}")
    #    message("A_DEPENDS: ${A_DEPENDS}")

        get_filename_component(A_OUTPUT_absolute ${DCI_OUT_DIR}/include/${A_OUTPUT} ABSOLUTE)

        get_filename_component(A_OUTPUT_absolute_path ${A_OUTPUT_absolute} DIRECTORY)
        file(MAKE_DIRECTORY ${A_OUTPUT_absolute_path})

        dciIntegrationMeta(UNIT ${uname} INCLUDE_FILE ${A_OUTPUT_absolute} include/${A_OUTPUT})

        ############################################################
        set(srcContent "")
        set(srcContent "${srcContent}#include <dci/himpl/generator/implMetaInfoWriter.hpp>\n")

        if(A_PREDECL)
            set(srcContent "${srcContent}#include \"${A_PREDECL}\"\n")
        endif()
        foreach(header ${A_HEADERS})
            set(srcContent "${srcContent}#include \"${header}\"\n")
        endforeach()

        set(srcContent "${srcContent}#include <iostream>\n")


        ############################################################
        set(srcContent "${srcContent}namespace dci::himpl::generator {\n")
        foreach(class ${A_CLASSES})
            set(srcContent "${srcContent}template <> const char* nameMapper< ${class} >() { return \"${class}\";}\n")
        endforeach()
        set(srcContent "${srcContent}}\n")


        set(srcContent "${srcContent}int main(){\n")
        set(srcContent "${srcContent}    std::cout<<\n")
        set(srcContent "${srcContent}        \"//automatic generated\\n\"\n")
        set(srcContent "${srcContent}        \"#pragma once\\n\"\n")
        set(srcContent "${srcContent}        \"\\n\"\n")
        set(srcContent "${srcContent}        \"#include <dci/himpl/implMetaInfo.hpp>\\n\"\n")
        set(srcContent "${srcContent}        \"\\n\"\n")

        ############################################################
        if(A_PREDECL)
            set(srcContent "${srcContent}        \"#include \\\"${A_PREDECL}\\\"\\n\"\n")
        else()
            foreach(class ${A_CLASSES})

                string(REPLACE "::" ";" namespaces ${class})
                list(REMOVE_ITEM namespaces "")
                list(REVERSE namespaces)
                list(GET namespaces 0 classWithoutNamespace)
                list(REMOVE_AT namespaces 0)
                list(REVERSE namespaces)

                set(srcContent "${srcContent}        \"namespace ")
                set(doSeparate)
                foreach(namespace ${namespaces})
                    if(NOT doSeparate)
                        set(doSeparate On)
                        set(srcContent "${srcContent}${namespace}")
                    else()
                        set(srcContent "${srcContent}::${namespace}")
                    endif()
                endforeach()
                set(srcContent "${srcContent} { ")

                set(srcContent "${srcContent}class ${classWithoutNamespace}; ")

                set(srcContent "${srcContent}}")

                set(srcContent "${srcContent}\\n\"\n")
            endforeach()
        endif()

        set(srcContent "${srcContent}        \"\\n\"\n")
        set(srcContent "${srcContent}        \"namespace dci::himpl\\n{\\n\\n\";\n\n")


        ############################################################

        set(classesCommaStr)
        foreach(class ${A_CLASSES})
            if(classesCommaStr)
                set(classesCommaStr "${classesCommaStr},\n         ${class}")
            else()
                set(classesCommaStr "\n         ${class}")
            endif()
        endforeach()

        set(srcContent "${srcContent}    dci::himpl::generator::ImplMetaInfoWriter< ${classesCommaStr} >::template exec< ${classesCommaStr} >();\n\n")


        set(srcContent "${srcContent}    std::cout<<\n")
        set(srcContent "${srcContent}        \"}\\n\"\n")

        set(srcContent "${srcContent}        \"\\n\";\n\n")
        set(srcContent "${srcContent}    return 0;\n")
        set(srcContent "${srcContent}}\n")





        ############################################################
        set(srcSkeleton "${CMAKE_CURRENT_BINARY_DIR}/${target}/implMetaInfoGen.skeleton")
        file(GENERATE OUTPUT ${srcSkeleton} CONTENT "${srcContent}")

        ############################################################
        set(src "${CMAKE_CURRENT_BINARY_DIR}/${target}/implMetaInfoGen.cpp")
        file(RELATIVE_PATH path4Comment ${CMAKE_BINARY_DIR} ${src})
        add_custom_command(OUTPUT ${src}
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different ${srcSkeleton} ${src}
                           DEPENDS ${srcSkeleton}
                           COMMENT "Generating ${path4Comment}"
        )

        set(implMetaInfoGen ${target}-implMetaInfoGen)
        add_executable(${implMetaInfoGen} ${src})
        dciIntegrationSetupTarget(${implMetaInfoGen} AUX)

        if(A_INCLUDE)
            target_include_directories(${implMetaInfoGen} SYSTEM PRIVATE ${A_INCLUDE})
        endif()

        if(A_LINK)
            target_link_libraries(${implMetaInfoGen} PRIVATE ${A_LINK})
        endif()

        if(A_DEPENDS)
            add_dependencies(${implMetaInfoGen} ${A_DEPENDS})
        endif()

        ############################################################
        file(RELATIVE_PATH path4Comment ${CMAKE_BINARY_DIR} ${A_OUTPUT_absolute})
        add_custom_command(OUTPUT ${A_OUTPUT_absolute}
                           COMMAND ${implMetaInfoGen}-cmd > ${A_OUTPUT_absolute}
                           DEPENDS ${implMetaInfoGen} ${implMetaInfoGen}-cmd
                           COMMENT "Generating ${path4Comment}"
        )

        target_sources(${target} PRIVATE ${A_OUTPUT_absolute})

    endfunction(dciHimplMakeLayouts)
endif()
