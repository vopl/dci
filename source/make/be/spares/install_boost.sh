#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh boost https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.tar.bz2 f0397ba6e982c4450f27bf32a2a83292aba035b827a5623a14636ea583318c41
cd ${WDIR}/boost

if [ ! -f "install.stamp" ]; then

    mkdir -p stage
    pushd boost_1_76_0
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
