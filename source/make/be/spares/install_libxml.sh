#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh libxml https://download.gnome.org/sources/libxml2/2.13/libxml2-2.13.6.tar.xz f453480307524968f7a04ec65e64f2a83a825973bcd260a2e7691be82ae70c96
cd ${WDIR}/libxml

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../libxml2-2.13.6/configure --prefix=${PREFIX} --libdir=${LIBDIR} --without-python
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
