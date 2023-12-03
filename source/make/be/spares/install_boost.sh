#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh boost https://boostorg.jfrog.io/artifactory/main/release/1.83.0/source/boost_1_83_0.tar.bz2 6478edfe2f3305127cffe8caf73ea0176c53769f4bf1585be237eb30798c3b8e
cd ${WDIR}/boost

if [ ! -f "install.stamp" ]; then

    mkdir -p stage
    pushd boost_1_83_0
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
