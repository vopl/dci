include(dciIntegrationUnit)
dciIntegrationUnit(aup
        INCLUDE_DIRS include
        DEPENDS poll stiac config
        WANT_POSTPROCESSING)

if(dciIntegrationStage2)
    ############################################################
    file(GLOB_RECURSE INC include/*)
    file(GLOB_RECURSE SRC src/*)
    file(GLOB_RECURSE TST test/*)

    add_library(${UNAME} SHARED ${INC} ${SRC})
    dciIntegrationSetupTarget(${UNAME})
    target_include_directories(${UNAME} PRIVATE src)

    target_link_libraries(${UNAME} PRIVATE
        integration
        exception
        utils
        mm
        bytes
        crypto
        sbs
        poll
        logger
        config
    )

    include(dciHimpl)
    dciHimplMakeLayouts(${UNAME}
        INCLUDE
            src

        OUTPUT
            dci/aup/implMetaInfo.hpp

        HEADERS
            impl/storage.hpp
            impl/catalog.hpp
            impl/applier.hpp

        CLASSES
            dci::aup::impl::Storage
            dci::aup::impl::Catalog
            dci::aup::impl::Applier
    )

    ############################################################
    set(conf etc/aup.conf.example)
    dciIntegrationMeta(UNIT ${UNAME} TARGET ${UNAME} RESOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${conf} ${conf})
    add_custom_command(
        OUTPUT ${DCI_OUT_DIR}/${conf}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${conf} ${DCI_OUT_DIR}/${conf}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${conf}
        COMMENT "Copying ${DCI_OUT_DIR}/${conf}")
    target_sources(${UNAME} PRIVATE ${DCI_OUT_DIR}/${conf})

    ############################################################
    include(dciTest)
    dciTest(${UNAME} noenv
        SRC ${TST}
        LINK ${UNAME} exception bytes utils crypto logger
    )

    ############################################################
    file(GLOB_RECURSE SRC_COLLECTOR src-collector/*)
    add_executable(${UNAME}-collector ${SRC_COLLECTOR})
    dciIntegrationSetupTarget(${UNAME}-collector BDEP)

    target_link_libraries(${UNAME}-collector PRIVATE
        ${UNAME}
        integration
        exception
        bytes
        crypto
        logger
    )

    find_package(Boost REQUIRED COMPONENTS program_options)
    target_link_libraries(${UNAME}-collector PRIVATE Boost::program_options)
endif()

if(dciIntegrationStage3)
    set(imageDir ${DCI_OUT_DIR}/var/aup)
    set(imageCatalogFile ${imageDir}/catalog)
    set(metaFile ${CMAKE_CURRENT_BINARY_DIR}/meta)

    #####################
    add_custom_command(OUTPUT ${metaFile}
        COMMAND ${CMAKE_COMMAND} -E remove -f ${metaFile}
        COMMAND ${CMAKE_COMMAND} -E touch ${metaFile}
        COMMENT "Collecting meta info in ${metaFile}"
        VERBATIM)

    get_property(commands GLOBAL PROPERTY dciIntegrationMeta_commands)
    get_property(deps GLOBAL PROPERTY dciIntegrationMeta_deps)

    foreach(cname ${commands})
        get_property(command GLOBAL PROPERTY ${cname})
        add_custom_command(OUTPUT ${metaFile} APPEND COMMAND sh -c ${command} >> ${metaFile})
    endforeach()

    foreach(dep ${deps})
        add_custom_command(OUTPUT ${metaFile} APPEND DEPENDS ${dep})
    endforeach()

    add_custom_command(OUTPUT ${imageCatalogFile}
        COMMAND ${UNAME}-collector-cmd
            --build-dir=${CMAKE_CURRENT_BINARY_DIR}
            --meta-file=${metaFile}
            --vendor-key=${DCI_VENDOR_KEY}
            --storage-dir=${imageDir}
            #--with-sources
            --with-debug-for-targets
            --with-debug-for-others
        DEPENDS ${UNAME}-collector ${UNAME}-collector-cmd ${metaFile}
        COMMENT "Collecting aup image in ${imageDir}"
        VERBATIM)

    add_custom_target(${UNAME}-image DEPENDS ${imageCatalogFile})
endif()
