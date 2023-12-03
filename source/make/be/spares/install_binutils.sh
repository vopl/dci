#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

stage=$1
if [[ "$stage" == "" ]]; then
    stage=1
fi

#################################
${CDIR}/prepareBuild.sh binutils https://mirror.kumi.systems/gnu/binutils/binutils-2.41.tar.xz ae9a5789e23459e59606e6714723f2d3ffc31c03174191ef0d015bdf06007450
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
        ../binutils-2.41/configure --prefix=${PREFIX} --libdir=${LIBDIR} \
            --disable-multilib --disable-multiarch \
            --enable-gold=yes --enable-ld=yes \
            --enable-gprofng=yes --enable-compressed-debug-sections=all --enable-default-compressed-debug-sections-algorithm=zstd \
            --enable-year2038 --enable-pgo-build=lto \
            --enable-lto --enable-vtable-verify \
            --disable-bootstrap \
            --enable-languages=c,c++,lto \
            --enable-plugin \
            --enable-shared

        make -j`nproc`
        make install
    popd

    touch install-${stage}.stamp
fi
