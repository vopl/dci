#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh pkg-config https://pkgconfig.freedesktop.org/releases/pkg-config-0.29.2.tar.gz 6fc69c01688c9458a57eb9a1664c9aba372ccda420a02bf4429fe610e7e7d591
cd ${WDIR}/pkg-config

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../pkg-config-0.29.2/configure \
        --prefix=${PREFIX} --libdir=${LIBDIR} \
        --with-system_include_path=${PREFIX}/include \
        --with-system_library_path=${PREFIX}/lib \
        --with-internal-glib \

    make -j`nproc` VERBOSE=1
    make install

    popd

    touch install.stamp
fi
