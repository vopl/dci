#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh chrpath https://alioth-archive.debian.org/releases/chrpath/chrpath/0.16/chrpath-0.16.tar.gz bb0d4c54bac2990e1bdf8132f2c9477ae752859d523e141e72b3b11a12c26e7b
cd ${WDIR}/chrpath

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../chrpath-0.16/configure --prefix=${PREFIX} --libdir=${LIBDIR}
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
