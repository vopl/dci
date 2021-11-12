#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh libxml ftp://xmlsoft.org/libxml2/libxml2-2.9.10.tar.gz aafee193ffb8fe0c82d4afef6ef91972cbaf5feea100edc2f262750611b4be1f
cd ${WDIR}/libxml

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../libxml2-2.9.10/configure --prefix=${PREFIX} --libdir=${LIBDIR} --without-python
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
