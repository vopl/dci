#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh glibc https://ftp.gnu.org/gnu/glibc/glibc-2.34.tar.xz 5123732f6b67ccd319305efd399971d58592122bcc2a6518a1bd2510dd0cf52e
cd ${WDIR}/glibc

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    export LDFLAGS="${LOCAL_LDFLAGS}"
    export CFLAGS="${LOCAL_CFLAGS}"
    export CXXFLAGS="${LOCAL_CXXFLAGS}"

    FAKE_PREFIX=/tmp/glibc-fake-prefix

    ../glibc-2.34/configure --prefix=${FAKE_PREFIX} \
        --disable-werror \
        --includedir=${PREFIX}/include \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
