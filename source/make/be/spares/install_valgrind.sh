#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh valgrind https://sourceware.org/pub/valgrind/valgrind-3.19.0.tar.bz2 dd5e34486f1a483ff7be7300cc16b4d6b24690987877c3278d797534d6738f02
cd ${WDIR}/valgrind

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../valgrind-3.19.0/configure --prefix=${PREFIX} --libdir=${LIBDIR} --enable-only64bit --without-mpicc
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
