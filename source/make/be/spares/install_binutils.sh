#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

stage=$1
if [[ "$stage" == "" ]]; then
    stage=1
fi

#################################
${CDIR}/prepareBuild.sh binutils https://ftp.gnu.org/gnu/binutils/binutils-2.35.tar.xz 1b11659fb49e20e18db460d44485f09442c8c56d5df165de9461eb09c8302f85
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
        ../binutils-2.35/configure --prefix=${PREFIX} --libdir=${LIBDIR} \
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
