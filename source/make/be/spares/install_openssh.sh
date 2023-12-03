#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh openssh https://cloudflare.cdn.openbsd.org/pub/OpenBSD/OpenSSH/portable/openssh-9.5p1.tar.gz f026e7b79ba7fb540f75182af96dc8a8f1db395f922bbc9f6ca603672686086b
cd ${WDIR}/openssh

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../openssh-9.5p1/configure \
        --prefix=${PREFIX} --libdir=${LIBDIR} --sysconfdir=${PREFIX}/etc/ssh \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
