#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

stage=$1
if [[ "$stage" == "" ]]; then
    stage=1
fi

#################################
${CDIR}/prepareBuild.sh zstd https://github.com/facebook/zstd/releases/download/v1.5.7/zstd-1.5.7.tar.gz eb33e51f49a15e023950cd7825ca74a4a2b43db8354825ac24fc1b7ee09e6fa3
cd ${WDIR}/zstd

if [ ! -f "install-${stage}.stamp" ]; then

    rm -rf build-${stage}
    mkdir -p build-${stage}
    cp -r zstd-1.5.7/* build-${stage}/
    pushd build-${stage}
        CC=gcc CFLAGS="${LOCAL_CFLAGS}" LDFLAGS="${LOCAL_LDFLAGS}" AR=gcc-ar NM=gcc-nm make -j`nproc`
        make prefix=${PREFIX} libdir=${LIBDIR} install
    popd

    touch install-${stage}.stamp
fi
