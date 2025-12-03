include(dciIntegrationUnit)
dciIntegrationUnit(crypto
        INCLUDE_DIRS include
        DEPENDS himpl test utils)

enable_language(C ASM)

##############################################################
file(GLOB_RECURSE INC include/*)
file(GLOB_RECURSE SRC src/*)

if(WIN32)
    list(FILTER SRC EXCLUDE REGEX ".*source.devRandom.(hpp|cpp)$")
    list(FILTER SRC EXCLUDE REGEX ".*source.devUrandom.(hpp|cpp)$")
else()
    list(FILTER SRC EXCLUDE REGEX ".*source.std.(hpp|cpp)$")
endif()

add_library(${UNAME} SHARED ${INC} ${SRC})
dciIntegrationSetupTarget(${UNAME})
#target_include_directories(${UNAME} PRIVATE src)

target_link_libraries(${UNAME} PRIVATE
    utils
)

include(dciHimpl)
dciHimplMakeLayouts(${UNAME}
    INCLUDE
        src

    OUTPUT
        dci/crypto/implMetaInfo.hpp

    HEADERS
        impl/hash.hpp
        impl/sha2_256.hpp
        impl/sha2_512.hpp
        impl/blake2b.hpp
        impl/blake2s.hpp
        impl/blake3.hpp
        impl/mac.hpp
        impl/hmac.hpp
        impl/poly1305.hpp
        impl/streamCipher.hpp
        impl/chaCha.hpp
        impl/chaCha20Poly1305.hpp

    CLASSES
        dci::crypto::impl::Hash
        dci::crypto::impl::Sha2_256
        dci::crypto::impl::Sha2_512
        dci::crypto::impl::Blake2b
        dci::crypto::impl::Blake2s
        dci::crypto::impl::Blake3
        dci::crypto::impl::Mac
        dci::crypto::impl::Hmac
        dci::crypto::impl::Poly1305
        dci::crypto::impl::StreamCipher
        dci::crypto::impl::ChaCha
        dci::crypto::impl::ChaCha20Poly1305
    )

file(GLOB_RECURSE TST test/*)
include(dciTest)
dciTest(${UNAME} noenv
    SRC ${TST}
    LINK ${UNAME} utils
)
