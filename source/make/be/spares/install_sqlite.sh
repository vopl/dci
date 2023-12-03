#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh sqlite https://sqlite.org/2023/sqlite-autoconf-3440200.tar.gz 1c6719a148bc41cf0f2bbbe3926d7ce3f5ca09d878f1246fcc20767b175bb407
cd ${WDIR}/sqlite

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS} -DSQLITE_ENABLE_COLUMN_METADATA=1" CXXFLAGS="${LOCAL_CXXFLAGS}" ../sqlite-autoconf-3440200/configure --prefix=${PREFIX} --libdir=${LIBDIR}
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
