#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh openssl https://www.openssl.org/source/openssl-3.2.0.tar.gz 14c826f07c7e433706fb5c69fa9e25dab95684844b4c962a2cf1bf183eb4690e
cd ${WDIR}/openssl

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../openssl-3.2.0/Configure \
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
