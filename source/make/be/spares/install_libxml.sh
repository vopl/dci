#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh libxml https://download.gnome.org/sources/libxml2/2.12/libxml2-2.12.1.tar.xz 8982b9ccdf7f456e30d8f7012d50858c6623e495333b6191def455c7e95427eb
cd ${WDIR}/libxml

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../libxml2-2.12.1/configure --prefix=${PREFIX} --libdir=${LIBDIR} --without-python
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
