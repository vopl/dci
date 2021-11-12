#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh curl https://curl.se/download/curl-7.78.0.tar.xz be42766d5664a739c3974ee3dfbbcbe978a4ccb1fe628bb1d9b59ac79e445fb5
cd ${WDIR}/curl

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../curl-7.78.0/configure \
        --prefix=${PREFIX} --libdir=${LIBDIR} --with-openssl \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
