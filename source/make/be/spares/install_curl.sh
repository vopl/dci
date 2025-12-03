#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh curl https://curl.se/download/curl-8.12.1.tar.xz 0341f1ed97a26c811abaebd37d62b833956792b7607ea3f15d001613c76de202
cd ${WDIR}/curl

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../curl-8.12.1/configure \
        --prefix=${PREFIX} --libdir=${LIBDIR} --with-openssl \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
