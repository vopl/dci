#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh boost https://boostorg.jfrog.io/artifactory/main/release/1.80.0/source/boost_1_80_0.tar.bz2 1e19565d82e43bc59209a168f5ac899d3ba471d55c7610c677d4ccf2c9c500c0
cd ${WDIR}/boost

if [ ! -f "install.stamp" ]; then

    mkdir -p stage
    pushd boost_1_80_0
        ./bootstrap.sh --prefix=${PREFIX} --libdir=${LIBDIR}
        ./b2 \
            --prefix=${PREFIX} --libdir=${LIBDIR} \
            --stagedir=../stage --build-dir=../build \
            --build-type=minimal --layout=system \
            --without-mpi --without-graph_parallel --without-python \
            --ignore-site-config \
            -s NO_BZIP2=1 -s NO_LZMA=1 \
            variant=release link=shared threading=multi runtime-link=shared \
            cxxflags="-fPIC ${LOCAL_CXXFLAGS}" cflags="-fPIC ${LOCAL_CFLAGS}" linkflags="${LOCAL_LDFLAGS}" \
            instruction-set=core2 \
            install
    popd
    touch install.stamp
fi
