#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh zstd https://github.com/facebook/zstd/releases/download/v1.4.9/zstd-1.4.9.tar.gz 29ac74e19ea28659017361976240c4b5c5c24db3b89338731a6feb97c038d293
cd ${WDIR}/zstd

if [ ! -f "install.stamp" ]; then

    pushd zstd-1.4.9
        CC=gcc CFLAGS="${LOCAL_CFLAGS}" LDFLAGS="${LOCAL_LDFLAGS}" AR=gcc-ar NM=gcc-nm make -j`nproc`
        make prefix=${PREFIX} libdir=${LIBDIR} install
    popd

    touch install.stamp
fi
