#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh boost https://archives.boost.io/release/1.87.0/source/boost_1_87_0.tar.bz2 af57be25cb4c4f4b413ed692fe378affb4352ea50fbe294a11ef548f4d527d89
cd ${WDIR}/boost

if [ ! -f "install.stamp" ]; then

    mkdir -p stage
    pushd boost_1_87_0
        ./bootstrap.sh --prefix=${PREFIX} --libdir=${LIBDIR} \
            --with-libraries=all \
            --without-libraries=graph_parallel,mpi \

        ./b2 \
            --build-type=minimal --layout=system \
            --stagedir=../stage --build-dir=../build \
            variant=release link=shared threading=multi runtime-link=shared \
            cxxflags="-fPIC ${LOCAL_CXXFLAGS}" cflags="-fPIC ${LOCAL_CFLAGS}" linkflags="${LOCAL_LDFLAGS}" \
            instruction-set=core2 \
            install

    popd
    touch install.stamp
fi
