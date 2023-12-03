#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh valgrind https://sourceware.org/pub/valgrind/valgrind-3.22.0.tar.bz2 c811db5add2c5f729944caf47c4e7a65dcaabb9461e472b578765dd7bf6d2d4c
cd ${WDIR}/valgrind

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../valgrind-3.22.0/configure --prefix=${PREFIX} --libdir=${LIBDIR} --enable-only64bit --without-mpicc
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
