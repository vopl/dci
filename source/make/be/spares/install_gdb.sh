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

    export LDFLAGS="${LOCAL_LDFLAGS}"
    export CFLAGS="${LOCAL_CFLAGS}"
    export CXXFLAGS="${LOCAL_CXXFLAGS}"
    export DEBUGINFOD_CFLAGS="${LOCAL_LDFLAGS} ${LOCAL_CFLAGS}"
    export DEBUGINFOD_LIBS="${LOCAL_LDFLAGS} -ldebuginfod"
    
    ../gdb-12.1/configure --prefix=${PREFIX} --libdir=${LIBDIR} \
        --enable-gold=yes \
        --enable-ld=yes \
        --enable-compressed-debug-sections=all \
        --with-python \
        --with-debuginfod
    
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
