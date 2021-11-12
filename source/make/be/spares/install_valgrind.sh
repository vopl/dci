#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh valgrind https://sourceware.org/pub/valgrind/valgrind-3.17.0.tar.bz2 ad3aec668e813e40f238995f60796d9590eee64a16dff88421430630e69285a2
cd ${WDIR}/valgrind

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../valgrind-3.17.0/configure --prefix=${PREFIX} --libdir=${LIBDIR} --enable-only64bit --without-mpicc
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
