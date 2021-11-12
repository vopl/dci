#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh gtest https://github.com/google/googletest/archive/release-1.10.0.tar.gz 9dc9157a9a1551ec7a7e43daea9a694a0bb5fb8bec81235d8a1e6ef64c716dcb
cd ${WDIR}/gtest

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" cmake ../googletest-release-1.10.0 -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_INSTALL_LIBDIR=${LIBDIR} -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=On
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
