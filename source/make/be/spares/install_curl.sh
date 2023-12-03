#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh curl https://curl.se/download/curl-8.4.0.tar.xz 16c62a9c4af0f703d28bda6d7bbf37ba47055ad3414d70dec63e2e6336f2a82d
cd ${WDIR}/curl

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../curl-8.4.0/configure \
        --prefix=${PREFIX} --libdir=${LIBDIR} --with-openssl \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
