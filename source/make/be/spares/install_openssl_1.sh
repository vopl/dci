#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh openssl https://www.openssl.org/source/openssl-1.1.1q.tar.gz d7939ce614029cdff0b6c20f0e2e5703158a489a72b2507b8bd51bf8c8fd10ca
cd ${WDIR}/openssl

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../openssl-1.1.1q/Configure \
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
