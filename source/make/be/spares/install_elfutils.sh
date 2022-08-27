#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh elfutils https://sourceware.org/elfutils/ftp/0.187/elfutils-0.187.tar.bz2 e70b0dfbe610f90c4d1fe0d71af142a4e25c3c4ef9ebab8d2d72b65159d454c8
cd ${WDIR}/elfutils

if [ ! -f "install-${stage}.stamp" ]; then

    mkdir -p build
    pushd build
        CFLAGS="${LOCAL_LDFLAGS} ${LOCAL_CFLAGS}" \
        CXXFLAGS="${LOCAL_LDFLAGS} ${LOCAL_CXXFLAGS}" \
        LDFLAGS="${LOCAL_LDFLAGS} -pthread" \
        libcurl_CFLAGS="${LOCAL_LDFLAGS} ${LOCAL_CFLAGS}" \
        libcurl_LIBS="${LOCAL_LDFLAGS} -lcurl" \
        sqlite3_CFLAGS="${LOCAL_LDFLAGS} ${LOCAL_CFLAGS}" \
        sqlite3_LIBS="${LOCAL_LDFLAGS} -lsqlite3" \
        libarchive_CFLAGS="${LOCAL_LDFLAGS} ${LOCAL_CFLAGS}" \
        libarchive_LIBS="${LOCAL_LDFLAGS} -larchive" \
        libmicrohttpd_CFLAGS="${LOCAL_LDFLAGS} ${LOCAL_CFLAGS}" \
        libmicrohttpd_LIBS="${LOCAL_LDFLAGS} -lmicrohttpd" \
        ../elfutils-0.187/configure --prefix=${PREFIX} --libdir=${LIBDIR} \
          --enable-deterministic-archives \
          --enable-thread-safety \
          --enable-dependency-tracking \
          --enable-debugpred \
          --enable-gprof \
          --enable-gcov \
          --enable-valgrind \
          --enable-valgrind-annotations \
          --enable-install-elfh \
          --enable-libdebuginfod \
          --enable-debuginfod \
          --with-valgrind \
          --with-zlib \
          --with-bzlib \
          --with-zstd \

          #--with-lzma \

        make -j`nproc`
        make install
    popd

    touch install.stamp
fi
