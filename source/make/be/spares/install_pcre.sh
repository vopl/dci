#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh pcre https://ftp.pcre.org/pub/pcre/pcre2-10.36.tar.bz2 a9ef39278113542968c7c73a31cfcb81aca1faa64690f400b907e8ab6b4a665c
cd ${WDIR}/pcre

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../pcre2-10.36/configure --prefix=${PREFIX} --libdir=${LIBDIR} \
        --enable-pcre2-16 \
        --enable-pcre2-32 \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
