#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh gtest https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz 81964fe578e9bd7c94dfdb09c8e4d6e6759e19967e397dbea48d1c10e45d0df2
cd ${WDIR}/gtest

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" cmake ../googletest-release-1.12.1 -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_INSTALL_LIBDIR=${LIBDIR} -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=On
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
