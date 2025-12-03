#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh pcre https://github.com/PCRE2Project/pcre2/releases/download/pcre2-10.45/pcre2-10.45.tar.bz2 21547f3516120c75597e5b30a992e27a592a31950b5140e7b8bfde3f192033c4
cd ${WDIR}/pcre

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../pcre2-10.45/configure --prefix=${PREFIX} --libdir=${LIBDIR} \
        --enable-pcre2-16 \
        --enable-pcre2-32 \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
