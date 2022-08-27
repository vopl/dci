#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh gdb https://ftp.gnu.org/gnu/gdb/gdb-12.1.tar.xz 0e1793bf8f2b54d53f46dea84ccfd446f48f81b297b28c4f7fc017b818d69fed
cd ${WDIR}/gdb

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../gdb-12.1/configure --prefix=${PREFIX} --libdir=${LIBDIR} --enable-gold=yes --enable-ld=yes --enable-compressed-debug-sections=all --with-python
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
