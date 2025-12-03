#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh libmpdec https://www.bytereef.org/software/mpdecimal/releases/mpdecimal-4.0.0.tar.gz 942445c3245b22730fd41a67a7c5c231d11cb1b9936b9c0f76334fb7d0b4468c
cd ${WDIR}/libmpdec

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../mpdecimal-4.0.0/configure \
        --prefix ${PREFIX} \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
