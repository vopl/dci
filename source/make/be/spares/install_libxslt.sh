#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh libxslt https://download.gnome.org/sources/libxslt/1.1/libxslt-1.1.43.tar.xz 5a3d6b383ca5afc235b171118e90f5ff6aa27e9fea3303065231a6d403f0183a
cd ${WDIR}/libxslt

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../libxslt-1.1.43/configure \
        --prefix=${PREFIX} \
        --with-python \
        --with-crypto \
        --with-plugins \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
