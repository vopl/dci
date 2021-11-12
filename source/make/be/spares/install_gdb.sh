#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh gdb https://ftp.gnu.org/gnu/gdb/gdb-10.1.tar.xz f82f1eceeec14a3afa2de8d9b0d3c91d5a3820e23e0a01bbb70ef9f0276b62c0
cd ${WDIR}/gdb

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../gdb-10.1/configure --prefix=${PREFIX} --libdir=${LIBDIR} --enable-gold=yes --enable-ld=yes --enable-compressed-debug-sections=all --with-python
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
