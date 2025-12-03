#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh libpsl https://github.com/rockdaboot/libpsl/releases/download/0.21.5/libpsl-0.21.5.tar.gz 1dcc9ceae8b128f3c0b3f654decd0e1e891afc6ff81098f227ef260449dae208
cd ${WDIR}/libpsl

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../libpsl-0.21.5/configure \
        --prefix=${PREFIX} --libdir=${LIBDIR}  \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
