#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh libgcrypt https://gnupg.org/ftp/gcrypt/libgcrypt/libgcrypt-1.11.0.tar.bz2 09120c9867ce7f2081d6aaa1775386b98c2f2f246135761aae47d81f58685b9c
cd ${WDIR}/libgcrypt

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../libgcrypt-1.11.0/configure --prefix=${PREFIX}
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
