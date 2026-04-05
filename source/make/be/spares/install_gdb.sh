#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh gdb https://mirror.kumi.systems/gnu/gdb/gdb-16.2.tar.xz 4002cb7f23f45c37c790536a13a720942ce4be0402d929c9085e92f10d480119
cd ${WDIR}/gdb

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    export LDFLAGS="${LOCAL_LDFLAGS}"
    export CFLAGS="${LOCAL_CFLAGS}"
    export CXXFLAGS="${LOCAL_CXXFLAGS}"
    #export DEBUGINFOD_CFLAGS="${LOCAL_LDFLAGS} ${LOCAL_CFLAGS}"
    #export DEBUGINFOD_LIBS="${LOCAL_LDFLAGS} -ldebuginfod"
    
    ../gdb-16.2/configure --prefix=${PREFIX} --libdir=${LIBDIR} \
        --disable-multilib --disable-multiarch \
        --enable-gold=yes --enable-ld=yes \
        --with-system-zlib --with-zstd \
        --with-system-readline \
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
