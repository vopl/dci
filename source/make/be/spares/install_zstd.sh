#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

stage=$1
if [[ "$stage" == "" ]]; then
    stage=1
fi

#################################
${CDIR}/prepareBuild.sh zstd https://github.com/facebook/zstd/releases/download/v1.5.5/zstd-1.5.5.tar.gz 9c4396cc829cfae319a6e2615202e82aad41372073482fce286fac78646d3ee4
cd ${WDIR}/zstd

if [ ! -f "install-${stage}.stamp" ]; then

    rm -rf build-${stage}
    mkdir -p build-${stage}
    cp -r zstd-1.5.5/* build-${stage}/
    pushd build-${stage}
        CC=gcc CFLAGS="${LOCAL_CFLAGS}" LDFLAGS="${LOCAL_LDFLAGS}" AR=gcc-ar NM=gcc-nm make -j`nproc`
        make prefix=${PREFIX} libdir=${LIBDIR} install
    popd

    touch install-${stage}.stamp
fi
