#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

stage=$1
if [[ "$stage" == "" ]]; then
    stage=1
fi

#################################
${CDIR}/prepareBuild.sh gcc https://mirror.kumi.systems/gnu/gcc/gcc-13.2.0/gcc-13.2.0.tar.xz e275e76442a6067341a27f04c5c6b83d8613144004c0413528863dc6b5c743da
cd ${WDIR}/gcc

if [ ! -f "prerequisites.stamp" ]; then
    (cd gcc-13.2.0 && ./contrib/download_prerequisites)
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
        ../gcc-13.2.0/configure --prefix=${PREFIX} --libdir=${LIBDIR} \
            --disable-multilib --disable-multiarch \
            --enable-gold=yes --enable-ld=yes \
            --enable-gprofng=yes --enable-compressed-debug-sections=all --enable-default-compressed-debug-sections-algorithm=zstd \
            --enable-year2038 \
            --enable-lto --enable-vtable-verify \
            --disable-bootstrap \
            --enable-languages=c,c++,lto \
            --enable-plugin \
            --enable-shared \
            --enable-threads --enable-tls --enable-__cxa_atexit

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
