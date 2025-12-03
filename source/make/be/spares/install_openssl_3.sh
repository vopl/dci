#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh openssl https://www.openssl.org/source/openssl-3.4.1.tar.gz 002a2d6b30b58bf4bea46c43bdd96365aaf8daa6c428782aa4feee06da197df3
cd ${WDIR}/openssl

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../openssl-3.4.1/Configure \
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
        no-docs \
        no-static-engine \
        no-asm \
        shared \
        threads \
        linux-x86_64 \
        -g \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
