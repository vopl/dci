#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh valgrind https://sourceware.org/pub/valgrind/valgrind-3.24.0.tar.bz2 71aee202bdef1ae73898ccf7e9c315134fa7db6c246063afc503aef702ec03bd
cd ${WDIR}/valgrind

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../valgrind-3.24.0/configure --prefix=${PREFIX} --libdir=${LIBDIR} --enable-only64bit --without-mpicc
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
