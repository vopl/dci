#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh curl https://curl.se/download/curl-7.84.0.tar.xz 2d118b43f547bfe5bae806d8d47b4e596ea5b25a6c1f080aef49fbcd817c5db8
cd ${WDIR}/curl

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../curl-7.84.0/configure \
        --prefix=${PREFIX} --libdir=${LIBDIR} --with-openssl \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
