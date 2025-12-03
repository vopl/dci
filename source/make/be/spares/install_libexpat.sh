#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh libexpat https://github.com/libexpat/libexpat/releases/download/R_2_7_0/expat-2.7.0.tar.gz 362e89ca6b8a0d46fc5740a917eb2a8b4d6356edbe016eee09f49c0781215844
cd ${WDIR}/libexpat

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../expat-2.7.0/configure \
        --prefix ${PREFIX} \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
