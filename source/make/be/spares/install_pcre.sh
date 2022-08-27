#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh pcre https://github.com/PCRE2Project/pcre2/releases/download/pcre2-10.40/pcre2-10.40.tar.bz2 14e4b83c4783933dc17e964318e6324f7cae1bc75d8f3c79bc6969f00c159d68
cd ${WDIR}/pcre

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../pcre2-10.40/configure --prefix=${PREFIX} --libdir=${LIBDIR} \
        --enable-pcre2-16 \
        --enable-pcre2-32 \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
