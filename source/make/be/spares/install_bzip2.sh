#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh bzip2 https://sourceware.org/pub/bzip2/bzip2-1.0.8.tar.gz ab5a03176ee106d3f0fa90e381da478ddae405918153cca248e682cd0c4a2269
cd ${WDIR}/bzip2

if [ ! -f "install.stamp" ]; then
    rm -rf build
    cp -r bzip2-1.0.8 build
    pushd build
        make install LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS} -Wall -Winline -O2 -g -D_FILE_OFFSET_BITS=64" PREFIX=${PREFIX}
        rm -f ${PREFIX}/lib/libbz2.a
    popd
    touch install.stamp
fi

if [ ! -f "install-so.stamp" ]; then
    rm -rf build-so
    cp -r bzip2-1.0.8 build-so
    pushd build-so
        make -f Makefile-libbz2_so all LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS} -fpic -fPIC -Wall -Winline -O2 -g -D_FILE_OFFSET_BITS=64" PREFIX=${PREFIX}
        cp -a libbz2.so.1.0.8 ${PREFIX}/lib
        cp -a libbz2.so.1.0 ${PREFIX}/lib
    popd
    touch install-so.stamp
fi
