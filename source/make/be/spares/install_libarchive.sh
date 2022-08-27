#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh libarchive https://libarchive.org/downloads/libarchive-3.6.1.tar.xz 5a411aceb978f43e626f0c2d1812ddd8807b645ed892453acabd532376c148e6
cd ${WDIR}/libarchive

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../libarchive-3.6.1/configure --prefix=${PREFIX} --libdir=${LIBDIR}
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
