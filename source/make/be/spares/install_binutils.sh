#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

stage=$1
if [[ "$stage" == "" ]]; then
    stage=1
fi

#################################
${CDIR}/prepareBuild.sh binutils https://ftp.gnu.org/gnu/binutils/binutils-2.39.tar.xz 645c25f563b8adc0a81dbd6a41cffbf4d37083a382e02d5d3df4f65c09516d00
cd ${WDIR}/binutils

if [ ! -f "install-${stage}.stamp" ]; then

    mkdir -p build-${stage}
    pushd build-${stage}
        CFLAGS="${LOCAL_LDFLAGS} ${LOCAL_CFLAGS}" \
        CXXFLAGS="${LOCAL_LDFLAGS} ${LOCAL_CXXFLAGS}" \
        LDFLAGS="${LOCAL_LDFLAGS}" \
        CFLAGS_FOR_TARGET="${LOCAL_LDFLAGS} ${LOCAL_CFLAGS}" \
        CXXFLAGS_FOR_TARGET="${LOCAL_LDFLAGS} ${LOCAL_CXXFLAGS}" \
        LDFLAGS_FOR_TARGET="${LOCAL_LDFLAGS}" \
        ../binutils-2.39/configure --prefix=${PREFIX} --libdir=${LIBDIR} \
            --disable-multilib --disable-multiarch \
            --enable-gold=yes --enable-ld=yes \
            --enable-compressed-debug-sections=all \
            --disable-bootstrap \
            --enable-languages=c,c++,lto \
            --enable-lto --enable-plugin \
            --enable-shared

        make -j`nproc`
        make install
    popd

    touch install-${stage}.stamp
fi
