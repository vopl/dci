#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh libmicrohttpd https://mirror.kumi.systems/gnu/libmicrohttpd/libmicrohttpd-0.9.77.tar.gz 9e7023a151120060d2806a6ea4c13ca9933ece4eacfc5c9464d20edddb76b0a0
cd ${WDIR}/libmicrohttpd

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../libmicrohttpd-0.9.77/configure --prefix=${PREFIX} --libdir=${LIBDIR}
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
