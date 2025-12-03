#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh libffi https://github.com/libffi/libffi/releases/download/v3.4.7/libffi-3.4.7.tar.gz 138607dee268bdecf374adf9144c00e839e38541f75f24a1fcf18b78fda48b2d
cd ${WDIR}/libffi

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../libffi-3.4.7/configure \
        --prefix ${PREFIX} \
        --enable-portable-binary \
        --disable-docs \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
