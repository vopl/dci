include(dciIntegrationUnit)
dciIntegrationUnit(module-ppn-service-dht
        IDL_DIRS idl
        DEPENDS host)

############################################################
file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE IDL idl/*)

# TODO: убрать этот воркараунд в любой момент, сейчас он нужен для подвления false-positive ворнинга с подачи msys2/mingw gcc 15.2.0
set_source_files_properties(src/dht.cpp PROPERTIES COMPILE_OPTIONS -Wno-maybe-uninitialized)

add_library(${UNAME} MODULE ${INC} ${SRC} ${IDL})
target_include_directories(${UNAME} PRIVATE src)

##############################################################
include(dciIdl)
dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES ppn/service/dht.idl
    NAME ppn/service/dht
)

dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES ppn/service/dht.idl
    OPTIONS --cpp-no-entities --cpp-stiac-support --cpp-stiac-support-filter "^::dci::idl::gen::ppn::service::(d|D)ht"
    NAME stiac-support
)

##############################################################
include(dciUtilsPch)
dciUtilsPch(${UNAME} src/pch.hpp ${SRC})

############################################################
include(dciHostModule)
dciHostModule(${UNAME})
