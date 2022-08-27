#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh libxml https://download.gnome.org/sources/libxml2/2.10/libxml2-2.10.1.tar.xz 21a9e13cc7c4717a6c36268d0924f92c3f67a1ece6b7ff9d588958a6db9fb9d8
cd ${WDIR}/libxml

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../libxml2-2.10.1/configure --prefix=${PREFIX} --libdir=${LIBDIR} --without-python
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
