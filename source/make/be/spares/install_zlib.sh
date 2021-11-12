#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh zlib https://www.zlib.net/zlib-1.2.11.tar.xz 4ff941449631ace0d4d203e3483be9dbc9da454084111f97ea0a2114e19bf066
cd ${WDIR}/zlib

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../zlib-1.2.11/configure --prefix=${PREFIX} --libdir=${LIBDIR}
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
