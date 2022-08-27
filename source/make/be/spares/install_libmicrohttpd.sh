#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh libmicrohttpd https://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-0.9.75.tar.gz 9278907a6f571b391aab9644fd646a5108ed97311ec66f6359cebbedb0a4e3bb
cd ${WDIR}/libmicrohttpd

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../libmicrohttpd-0.9.75/configure --prefix=${PREFIX} --libdir=${LIBDIR}
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
