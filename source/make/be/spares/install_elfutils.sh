#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh elfutils https://sourceware.org/elfutils/ftp/0.190/elfutils-0.190.tar.bz2 8e00a3a9b5f04bc1dc273ae86281d2d26ed412020b391ffcc23198f10231d692
cd ${WDIR}/elfutils

if [ ! -f "install.stamp" ]; then

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
        ZSTD_COMPRESS_LIBS="${LOCAL_LDFLAGS} -lzstd" \
        ../elfutils-0.190/configure --prefix=${PREFIX} --libdir=${LIBDIR} \
          --enable-deterministic-archives \
          --enable-thread-safety \
          --enable-dependency-tracking \
          --enable-debugpred \
          --enable-gprof \
          --enable-gcov \
          --enable-valgrind \
          --enable-valgrind-annotations \
          --enable-install-elfh \
          --disable-libdebuginfod \
          --disable-debuginfod \
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
