#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh sqlite https://sqlite.org/2025/sqlite-autoconf-3490100.tar.gz 106642d8ccb36c5f7323b64e4152e9b719f7c0215acf5bfeac3d5e7f97b59254
cd ${WDIR}/sqlite

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS} -DSQLITE_ENABLE_COLUMN_METADATA=1" CXXFLAGS="${LOCAL_CXXFLAGS}" ../sqlite-autoconf-3490100/configure \
        --prefix=${PREFIX} \
        --with-tempstore=yes \
        --fts4 \
        --fts5 \
        --geopoly \
        --rtree \
        --session \
        --update-limit \
        --memsys5 \
        --scanstatus \
        --json \
        --with-icu-ldflags='-licui18n -licuuc -licudata' \
        --icu-collations \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
