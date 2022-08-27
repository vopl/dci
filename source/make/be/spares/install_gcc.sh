#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

stage=$1
if [[ "$stage" == "" ]]; then
    stage=1
fi

#################################
${CDIR}/prepareBuild.sh gcc https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.xz e549cf9cf3594a00e27b6589d4322d70e0720cdd213f39beb4181e06926230ff
cd ${WDIR}/gcc

if [ ! -f "prerequisites.stamp" ]; then
    (cd gcc-12.2.0 && ./contrib/download_prerequisites)
    touch prerequisites.stamp
fi

if [ ! -f "install-${stage}.stamp" ]; then

    mkdir -p build-${stage}
    pushd build-${stage}

        CFLAGS="${LOCAL_LDFLAGS} ${LOCAL_CFLAGS}" \
        CXXFLAGS="${LOCAL_LDFLAGS} ${LOCAL_CXXFLAGS}" \
        LDFLAGS="${LOCAL_LDFLAGS}" \
        CFLAGS_FOR_TARGET="${LOCAL_LDFLAGS} ${LOCAL_CFLAGS}" \
        CXXFLAGS_FOR_TARGET="${LOCAL_LDFLAGS} ${LOCAL_CXXFLAGS}" \
        LDFLAGS_FOR_TARGET="${LOCAL_LDFLAGS}" \
        ../gcc-12.2.0/configure --prefix=${PREFIX} --libdir=${LIBDIR} \
            --disable-multilib --disable-multiarch \
            --enable-gold=yes --enable-ld=yes \
            --enable-compressed-debug-sections=all \
            --disable-bootstrap \
            --enable-languages=c,c++,lto \
            --enable-lto --enable-plugin \
            --enable-shared --enable-threads --enable-tls --enable-__cxa_atexit

        make -j`nproc`
        make install

        if [[ ! -f ${PREFIX}/bin/cc ]]; then
            ln -s gcc ${PREFIX}/bin/cc
        fi

        if [[ ! -L ${PREFIX}/lib64 ]]; then
            mv ${PREFIX}/lib64/* ${PREFIX}/lib/
            rmdir ${PREFIX}/lib64
            ln -s lib ${PREFIX}/lib64
        fi

    popd

    touch install-${stage}.stamp
fi
