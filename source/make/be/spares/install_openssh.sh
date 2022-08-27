#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh openssh https://cloudflare.cdn.openbsd.org/pub/OpenBSD/OpenSSH/portable/openssh-9.0p1.tar.gz 03974302161e9ecce32153cfa10012f1e65c8f3750f573a73ab1befd5972a28a
cd ${WDIR}/openssh

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../openssh-9.0p1/configure \
        --prefix=${PREFIX} --libdir=${LIBDIR} --sysconfdir=${PREFIX}/etc/ssh \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
