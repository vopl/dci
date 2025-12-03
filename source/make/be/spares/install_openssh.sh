#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh openssh https://cloudflare.cdn.openbsd.org/pub/OpenBSD/OpenSSH/portable/openssh-9.9p2.tar.gz 91aadb603e08cc285eddf965e1199d02585fa94d994d6cae5b41e1721e215673
cd ${WDIR}/openssh

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../openssh-9.9p2/configure \
        --prefix=${PREFIX} --libdir=${LIBDIR} --sysconfdir=${PREFIX}/etc/ssh \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
