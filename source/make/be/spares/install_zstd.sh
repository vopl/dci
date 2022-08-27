#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh zstd https://github.com/facebook/zstd/releases/download/v1.5.2/zstd-1.5.2.tar.gz 7c42d56fac126929a6a85dbc73ff1db2411d04f104fae9bdea51305663a83fd0
cd ${WDIR}/zstd

if [ ! -f "install.stamp" ]; then

    pushd zstd-1.5.2
        CC=gcc CFLAGS="${LOCAL_CFLAGS}" LDFLAGS="${LOCAL_LDFLAGS}" AR=gcc-ar NM=gcc-nm make -j`nproc`
        make prefix=${PREFIX} libdir=${LIBDIR} install
    popd

    touch install.stamp
fi
