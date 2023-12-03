#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh gdb https://mirror.kumi.systems/gnu/gdb/gdb-14.1.tar.xz d66df51276143451fcbff464cc8723d68f1e9df45a6a2d5635a54e71643edb80
cd ${WDIR}/gdb

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    export LDFLAGS="${LOCAL_LDFLAGS}"
    export CFLAGS="${LOCAL_CFLAGS}"
    export CXXFLAGS="${LOCAL_CXXFLAGS}"
    #export DEBUGINFOD_CFLAGS="${LOCAL_LDFLAGS} ${LOCAL_CFLAGS}"
    #export DEBUGINFOD_LIBS="${LOCAL_LDFLAGS} -ldebuginfod"
    
    ../gdb-14.1/configure --prefix=${PREFIX} --libdir=${LIBDIR} \
        --disable-multilib --disable-multiarch \
        --enable-gold=yes --enable-ld=yes \
        --enable-gprofng=yes --enable-compressed-debug-sections=all --enable-default-compressed-debug-sections-algorithm=zstd \
        --enable-year2038 \
        --enable-languages=c,c++,lto \
        --enable-plugin \
        --enable-shared \
        --enable-threads --enable-tls \
        --with-python \

        #--with-debuginfod
    
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
