#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh openssl https://www.openssl.org/source/openssl-1.1.1k.tar.gz 892a0875b9872acd04a9fde79b1f943075d5ea162415de3047c327df33fbaee5
cd ${WDIR}/openssl

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../openssl-1.1.1k/Configure \
        --prefix=${PREFIX} --libdir=${LIBDIR} --openssldir=${PREFIX}/ssl \
        enable-camellia \
        enable-ec \
        enable-ec2m \
        enable-sm2 \
        enable-srp \
        enable-idea \
        enable-mdc2 \
        enable-rc4 \
        enable-rc5 \
        enable-ssl3 \
        enable-ssl3-method \
        enable-rfc3779 \
        enable-heartbeats \
        enable-zlib \
        no-static-engine \
        no-asm \
        no-hw \
        shared \
        threads \
        linux-x86_64 \
        -g \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
