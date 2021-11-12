#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh openssh https://cloudflare.cdn.openbsd.org/pub/OpenBSD/OpenSSH/portable/openssh-8.5p1.tar.gz f52f3f41d429aa9918e38cf200af225ccdd8e66f052da572870c89737646ec25
cd ${WDIR}/openssh

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../openssh-8.5p1/configure \
        --prefix=${PREFIX} --libdir=${LIBDIR} --sysconfdir=${PREFIX}/etc/ssh \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
