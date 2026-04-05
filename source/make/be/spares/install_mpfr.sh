#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh mpfr https://www.mpfr.org/mpfr-current/mpfr-4.2.2.tar.xz b67ba0383ef7e8a8563734e2e889ef5ec3c3b898a01d00fa0a6869ad81c6ce01
cd ${WDIR}/mpfr

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../mpfr-4.2.2/configure --prefix=${PREFIX} --libdir=${LIBDIR}
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
